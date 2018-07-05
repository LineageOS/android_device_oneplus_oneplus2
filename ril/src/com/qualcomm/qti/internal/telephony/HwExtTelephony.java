/*
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.ServiceManager;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.PhoneNumberUtils;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.uicc.IccCardStatus.CardState;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccController;

import org.codeaurora.internal.IDepersoResCallback;
import org.codeaurora.internal.IDsda;
import org.codeaurora.internal.IExtTelephony;

import java.util.Iterator;

import static android.telephony.TelephonyManager.SIM_ACTIVATION_STATE_ACTIVATED;
import static android.telephony.TelephonyManager.SIM_ACTIVATION_STATE_DEACTIVATED;

import static android.telephony.SubscriptionManager.INVALID_SUBSCRIPTION_ID;

import static com.android.internal.telephony.uicc.IccCardStatus.CardState.CARDSTATE_PRESENT;

public class HwExtTelephony extends IExtTelephony.Stub {

    // Service name
    private static final String EXT_TELEPHONY_SERVICE_NAME = "extphone";

    // Intents (+ extras) to broadcast
    private static final String ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED =
            "org.codeaurora.intent.action.ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED";
    private static final String EXTRA_NEW_PROVISION_STATE = "newProvisionState";

    // UICC States
    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;
    private static final int INVALID_STATE = -1;
    private static final int CARD_NOT_PRESENT = -2;

    // Error codes
    private static final int SUCCESS = 0;
    private static final int GENERIC_FAILURE = -1;
    private static final int INVALID_INPUT = -2;
    private static final int BUSY = -3;

    // From IccCardProxy.java
    private static final int EVENT_ICC_CHANGED = 3;

    private static CommandsInterface[] sCommandsInterfaces;
    private static Context sContext;
    private static HwExtTelephony sInstance;
    private static Phone[] sPhones;
    private static SubscriptionManager sSubscriptionManager;
    private static TelecomManager sTelecomManager;
    private static TelephonyManager sTelephonyManager;
    private static int sPreviousDataSlotId;
    private static int sUiccStatus[];

    private Handler mHandler;
    private UiccController mUiccController;

    public static void init(Context context, Phone[] phones, CommandsInterface[] commandsInterfaces) {
        sCommandsInterfaces = commandsInterfaces;
        sContext = context;
        sInstance = getInstance();
        sPhones = phones;
        sSubscriptionManager = (SubscriptionManager) sContext.getSystemService(
                Context.TELEPHONY_SUBSCRIPTION_SERVICE);
        sTelecomManager = TelecomManager.from(context);
        sTelephonyManager = TelephonyManager.from(context);
        sPreviousDataSlotId = -1;

        // Assume everything present is provisioned by default
        sUiccStatus = new int[sPhones.length];
        for (int i = 0; i < sPhones.length; i++) {
            if (sPhones[i] == null) {
                sUiccStatus[i] = INVALID_STATE;
            } else if (sPhones[i].getUiccCard() == null) {
                sUiccStatus[i] = CARD_NOT_PRESENT;
            } else {
                sUiccStatus[i] = sPhones[i].getUiccCard().getCardState() == CARDSTATE_PRESENT
                        ? PROVISIONED : CARD_NOT_PRESENT;
            }
        }
    }

    public static HwExtTelephony getInstance() {
        if (sInstance == null) {
            sInstance = new HwExtTelephony();
        }

        return sInstance;
    }

    private HwExtTelephony() {
        if (ServiceManager.getService(EXT_TELEPHONY_SERVICE_NAME) == null) {
            ServiceManager.addService(EXT_TELEPHONY_SERVICE_NAME, this);
        }

        // Keep track of ICC state
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                AsyncResult ar;

                if (msg.what == EVENT_ICC_CHANGED) {
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.result != null) {
                        iccStatusChanged((Integer) ar.result);
                    }
                }
            }
        };
        mUiccController = UiccController.getInstance();
        mUiccController.registerForIccChanged(mHandler, EVENT_ICC_CHANGED, null);
    }

    private void iccStatusChanged(int slotId) {
        if (slotId >= sPhones.length || sPhones[slotId] == null) {
            return;
        }

        UiccCard card = sPhones[slotId].getUiccCard();

        if (card == null) {
            sUiccStatus[slotId] = CARD_NOT_PRESENT;
            return;
        }

        if (sPreviousDataSlotId == slotId) {
            sUiccStatus[slotId] = PROVISIONED;
            deactivateUiccCard(slotId);
            sPreviousDataSlotId = -1;
        } else if (sUiccStatus[slotId] != NOT_PROVISIONED ||
                card.getCardState() != CARDSTATE_PRESENT) {
            if (card.getCardState() == CARDSTATE_PRESENT) {
                sUiccStatus[slotId] = NOT_PROVISIONED;
                activateUiccCard(slotId);
            } else {
                sUiccStatus[slotId] = CARD_NOT_PRESENT;
            }
        }

        broadcastUiccActivation(slotId);
    }

    private void setUiccActivation(int slotId, boolean activate) {
        UiccCard card = sPhones[slotId].getUiccCard();

        int numApps = card.getNumApplications();

        for (int i = 0; i < numApps; i++) {
            if (card.getApplicationIndex(i) == null) {
                continue;
            }

            sCommandsInterfaces[slotId].setUiccSubscription(i, activate, null);
        }
    }

    private void broadcastUiccActivation(int slotId) {
        Intent intent = new Intent(ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, slotId);
        intent.putExtra(EXTRA_NEW_PROVISION_STATE, sUiccStatus[slotId]);
        sContext.sendBroadcast(intent);
    }

    @Override
    public int getCurrentUiccCardProvisioningStatus(int slotId) {
        if (slotId >= sUiccStatus.length) {
            return INVALID_INPUT;
        }

        return sUiccStatus[slotId];
    }

    @Override
    public int getUiccCardProvisioningUserPreference(int slotId) {
        // I hope we don't use this
        return getCurrentUiccCardProvisioningStatus(slotId);
    }

    @Override
    public int activateUiccCard(int slotId) {
        if (slotId >= sPhones.length || sPhones[slotId] == null ||
                slotId >= sCommandsInterfaces.length || sCommandsInterfaces[slotId] == null) {
            return INVALID_INPUT;
        }

        if (sUiccStatus[slotId] == PROVISIONED) {
            return SUCCESS;
        }

        if (sUiccStatus[slotId] != NOT_PROVISIONED) {
            return INVALID_INPUT;
        }

        setUiccActivation(slotId, true);
        sPhones[slotId].setVoiceActivationState(SIM_ACTIVATION_STATE_ACTIVATED);
        sPhones[slotId].setDataActivationState(SIM_ACTIVATION_STATE_ACTIVATED);

        sUiccStatus[slotId] = PROVISIONED;
        broadcastUiccActivation(slotId);

        return SUCCESS;
    }

    @Override
    public int deactivateUiccCard(int slotId) {
        if (slotId >= sPhones.length || sPhones[slotId] == null ||
                slotId >= sCommandsInterfaces.length || sCommandsInterfaces[slotId] == null) {
            return INVALID_INPUT;
        }

        if (sUiccStatus[slotId] == NOT_PROVISIONED) {
            return SUCCESS;
        }

        if (sUiccStatus[slotId] != PROVISIONED) {
            return INVALID_INPUT;
        }

        int subIdToDeactivate = sPhones[slotId].getSubId();
        int subIdToMakeDefault = INVALID_SUBSCRIPTION_ID;

        // Find first provisioned sub that isn't what we're deactivating
        for (int i = 0; i < sPhones.length; i++) {
            if (i == slotId) {
                continue;
            }
            if (sUiccStatus[i] == PROVISIONED) {
                subIdToMakeDefault = sPhones[i].getSubId();
                break;
            }
        }

        // Make sure defaults are now sane
        PhoneAccountHandle accountHandle = sTelecomManager.getUserSelectedOutgoingPhoneAccount();
        PhoneAccount account = sTelecomManager.getPhoneAccount(accountHandle);

        if (sSubscriptionManager.getDefaultSmsSubscriptionId() == subIdToDeactivate) {
            sSubscriptionManager.setDefaultSmsSubId(subIdToMakeDefault);
        }

        if (sSubscriptionManager.getDefaultDataSubscriptionId() == subIdToDeactivate) {
            sPreviousDataSlotId = slotId;
            sSubscriptionManager.setDefaultDataSubId(subIdToMakeDefault);
        }

        if (sTelephonyManager.getSubIdForPhoneAccount(account) == subIdToDeactivate) {
            sTelecomManager.setUserSelectedOutgoingPhoneAccount(
                    subscriptionIdToPhoneAccountHandle(subIdToMakeDefault));
        }

        sPhones[slotId].setVoiceActivationState(SIM_ACTIVATION_STATE_DEACTIVATED);
        sPhones[slotId].setDataActivationState(SIM_ACTIVATION_STATE_DEACTIVATED);
        setUiccActivation(slotId, false);

        sUiccStatus[slotId] = NOT_PROVISIONED;
        broadcastUiccActivation(slotId);

        return SUCCESS;
    }

    @Override
    public boolean isSMSPromptEnabled() {
        // I hope we don't use this
        return false;
    }

    @Override
    public void setSMSPromptEnabled(boolean enabled) {
        // I hope we don't use this
    }

    @Override
    public int getPhoneIdForECall() {
        // I hope we don't use this
        return -1;
    }

    @Override
    public void setPrimaryCardOnSlot(int slotId) {
        // I hope we don't use this
    }

    @Override
    public boolean isFdnEnabled() {
        // I hope we don't use this
        return false;
    }

    @Override
    public int getPrimaryStackPhoneId() {
        // I hope we don't use this
        return -1;
    }

    @Override
    public boolean isEmergencyNumber(String number) {
        // This is lame...
        return PhoneNumberUtils.isEmergencyNumber(number);
    }

    @Override
    public boolean isLocalEmergencyNumber(String number) {
        // This is lame...
        return PhoneNumberUtils.isLocalEmergencyNumber(sContext, number);
    }

    @Override
    public boolean isPotentialEmergencyNumber(String number) {
        // This is lame...
        return PhoneNumberUtils.isPotentialEmergencyNumber(number);
    }

    @Override
    public boolean isPotentialLocalEmergencyNumber(String number) {
        // This is lame...
        return PhoneNumberUtils.isPotentialLocalEmergencyNumber(sContext, number);
    }

    @Override
    public boolean isDeviceInSingleStandby() {
        // I hope we don't use this
        return false;
    }

    @Override
    public boolean setLocalCallHold(int subId, boolean enable) {
        // TODO: Do something here
        return false;
    }

    @Override
    public void switchToActiveSub(int subId) {
        // I hope we don't use this
    }

    @Override
    public void setDsdaAdapter(IDsda dsdaAdapter) {
        // I hope we don't use this
    }

    @Override
    public int getActiveSubscription() {
        // I hope we don't use this
        return -1;
    }

    @Override
    public boolean isDsdaEnabled() {
        // I hope we don't use this
        return false;
    }

    @Override
    public void supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        // I hope we don't use this
    }

    @Override
    public int getPrimaryCarrierSlotId() {
        // I hope we don't use this
        return -1;
    }

    @Override
    public boolean isPrimaryCarrierSlotId(int slotId) {
        // I hope we don't use this
        return false;
    }

    @Override
    public boolean setSmscAddress(int slotId, String smsc) {
        // I hope we don't use this
        return false;
    }

    @Override
    public String getSmscAddress(int slotId) {
        // I hope we don't use this
        return null;
    }

    @Override
    public boolean isVendorApkAvailable(String packageName) {
        // I hope we don't use this
        return false;
    }

    @Override
    public int getCurrentPrimaryCardSlotId() {
        // I hope we don't use this
        return -1;
    }

    private PhoneAccountHandle subscriptionIdToPhoneAccountHandle(final int subId) {
        final Iterator<PhoneAccountHandle> phoneAccounts =
                sTelecomManager.getCallCapablePhoneAccounts().listIterator();

        while (phoneAccounts.hasNext()) {
            final PhoneAccountHandle phoneAccountHandle = phoneAccounts.next();
            final PhoneAccount phoneAccount = sTelecomManager.getPhoneAccount(phoneAccountHandle);
            if (subId == sTelephonyManager.getSubIdForPhoneAccount(phoneAccount)) {
                return phoneAccountHandle;
            }
        }

        return null;
    }

}
