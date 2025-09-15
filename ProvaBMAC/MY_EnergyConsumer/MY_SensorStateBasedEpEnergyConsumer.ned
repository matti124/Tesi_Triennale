//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

package MY_EnergyConsumer;
import inet.physicallayer.wireless.common.energyconsumer.*;//

//
simple MY_SensorStateBasedEpEnergyConsumer extends MY_StateBasedEpEnergyConsumer
{
    parameters:
        offPowerConsumption = default(0mW);
        sleepPowerConsumption = default(0.001mW);
        switchingPowerConsumption = default(25mW);
        receiverIdlePowerConsumption = default(0.005mW);
        receiverBusyPowerConsumption = default(0.1mW);
        receiverReceivingPowerConsumption = default(50mW);
        transmitterIdlePowerConsumption = default(5mW);
        transmitterTransmittingPowerConsumption = default(75mW);
}

