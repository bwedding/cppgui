// src/lib/subset-datastore.ts
import { atom } from 'jotai';
import { signal, computed } from "@preact/signals-react";

// Types
interface PressureSensor {
  MaxValue: string;
  MinValue: string;
  MeanValue: string;
  BackColor?: string;
}

interface HeartMetrics {
  StrokeVolume: string;
  PowerConsumption: PressureSensor;
  IntPressure: PressureSensor;
  MedicalPressure: PressureSensor;
  IntPressureMin: number;
  IntPressureMax: number;
  CardiacOutput: PressureSensor;
  ActualStrokeLen: string;
  TargetStrokeLen: string;
  SensorTemperature: string;
  ThermistorTemperature: string;
  CpuLoad: string;
  OutflowPressure: string;
}

interface SensorReadings {
  artPress?: PressureSensor;
  pap?: PressureSensor;
  cvp?: PressureSensor;
  aop?: PressureSensor;
  ivc?: PressureSensor;
}

// Signals
export const sensorReadingsSignal = signal<SensorReadings>({});
export const leftHeartSignal = signal<HeartMetrics | null>(null);
export const rightHeartSignal = signal<HeartMetrics | null>(null);
export const stopwatchSignal = signal<number>(0);

// Derived signals
export const atrialPressuresSignal = computed(() => {
  return {
    left: leftHeartSignal.value?.MedicalPressure,
    right: rightHeartSignal.value?.MedicalPressure
  };
});

export const cardiacOutputSignal = computed(() => {
  return {
    left: leftHeartSignal.value?.CardiacOutput,
    right: rightHeartSignal.value?.CardiacOutput
  };
});

// Import from jotai/utils
import { atomWithStorage } from 'jotai/utils';
import { getDefaultStore } from 'jotai';

// Initialize the default store
const store = getDefaultStore();

// Pressure card visibility
interface PressureCardVisibility {
  MAP: boolean;
  PAP: boolean;
  CVP: boolean;
  AOP: boolean;
  LAP: boolean;
  RAP: boolean;
  IVC: boolean;
  LCO: boolean;
  RCO: boolean;
}

export const pressureCardVisibilityAtom = atomWithStorage<PressureCardVisibility>('pressureCardVisibility', {
  MAP: true,
  PAP: true,
  CVP: true,
  AOP: true,
  LAP: true,
  RAP: true,
  IVC: true,
  LCO: true,
  RCO: true,
});

export const pressureCardVisibilitySignal = computed(() => store.get(pressureCardVisibilityAtom));

// Helper function to convert numeric pressure readings to string format
function convertPressureReading(reading: any): PressureSensor {
  if (!reading) {
    return { MaxValue: "0", MinValue: "0", MeanValue: "0" };
  }
  
  return {
    MaxValue: reading.MaxValue?.toString() || "0",
    MinValue: reading.MinValue?.toString() || "0",
    MeanValue: reading.MeanValue?.toString() || "0",
    BackColor: reading.BackColor
  };
}

// Initialize message handling
export function initializeMessageHandling(options: {
  setSensorReadings?: (readings: SensorReadings) => void;
  setLeftHeart?: (heart: HeartMetrics) => void;
  setRightHeart?: (heart: HeartMetrics) => void;
  setStopwatch?: (time: number) => void;
} = {}) {
  window.addEventListener('message', (event) => {
    const { type, data } = event.data;
    
    switch (type) {
      case 'UPDATE_SENSORS':
        console.log("Updating sensor readings:", data);
        // Convert numeric pressure readings to string format
        const convertedSensors: SensorReadings = {
          artPress: data.artPress ? convertPressureReading(data.artPress) : undefined,
          pap: data.pap ? convertPressureReading(data.pap) : undefined,
          cvp: data.cvp ? convertPressureReading(data.cvp) : undefined,
          aop: data.aop ? convertPressureReading(data.aop) : undefined,
          ivc: data.ivc ? convertPressureReading(data.ivc) : undefined
        };
        
        // Update signal directly
        sensorReadingsSignal.value = convertedSensors;
        
        // Also update atom if setter provided (for backward compatibility)
        if (options.setSensorReadings) {
          options.setSensorReadings(convertedSensors);
        }
        break;
      case 'UPDATE_LEFT_HEART':
      case 'UPDATE_RIGHT_HEART':
        console.log(`Updating ${type === 'UPDATE_LEFT_HEART' ? 'left' : 'right'} heart:`, data);
        // Create a full HeartMetrics object
        const heartMetrics: HeartMetrics = {
          StrokeVolume: "0",
          PowerConsumption: { MeanValue: "0", MinValue: "0", MaxValue: "0" },
          IntPressure: convertPressureReading(data.IntPressure),
          MedicalPressure: { MeanValue: "0", MinValue: "0", MaxValue: "0" },
          IntPressureMin: data.IntPressure.MinValue,
          IntPressureMax: data.IntPressure.MaxValue,
          CardiacOutput: { MeanValue: "0", MinValue: "0", MaxValue: "0" },
          ActualStrokeLen: "0",
          TargetStrokeLen: "0",
          SensorTemperature: "0",
          ThermistorTemperature: "0",
          CpuLoad: "0",
          OutflowPressure: "0"
        };
        
        // Update signal directly
        if (type === 'UPDATE_LEFT_HEART') {
          leftHeartSignal.value = heartMetrics;
          
          // Also update atom if setter provided (for backward compatibility)
          if (options.setLeftHeart) {
            options.setLeftHeart(heartMetrics);
          }
        } else {
          rightHeartSignal.value = heartMetrics;
          
          // Also update atom if setter provided (for backward compatibility)
          if (options.setRightHeart) {
            options.setRightHeart(heartMetrics);
          }
        }
        break;
      case 'UPDATE_STOPWATCH':
        // Update signal directly
        stopwatchSignal.value = data;
        
        // Also update atom if setter provided (for backward compatibility)
        if (options.setStopwatch) {
          options.setStopwatch(data);
        }
        break;
    }
  });
}