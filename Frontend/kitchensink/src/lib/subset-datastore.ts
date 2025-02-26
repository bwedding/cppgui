// src/lib/subset-datastore.ts
import { atom } from 'jotai';

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

// Create atoms with debug logging
const createLoggingAtom = <T>(initialValue: T, name: string) => {
  const baseAtom = atom<T>(initialValue);
  const writableAtom = atom(
    (get) => get(baseAtom),
    (get, set, update: T) => {
      console.log(`Updating ${name} atom:`, update);
      set(baseAtom, update);
      console.log(`New ${name} value:`, get(baseAtom));
    }
  );
  return writableAtom;
};

// Atoms
export const sensorReadingsAtom = createLoggingAtom<SensorReadings>({}, 'sensorReadings');
export const leftHeartAtom = createLoggingAtom<HeartMetrics | null>(null, 'leftHeart');
export const rightHeartAtom = createLoggingAtom<HeartMetrics | null>(null, 'rightHeart');
export const stopwatchAtom = createLoggingAtom<number>(0, 'stopwatch');

// Atrial Pressures atom for left and right pressure values
export const AtrialPressuresAtom = atom((get) => {
  const leftHeart = get(leftHeartAtom);
  const rightHeart = get(rightHeartAtom);
  return {
    left: leftHeart?.MedicalPressure,
    right: rightHeart?.MedicalPressure
  };
});

// Cardiac Output atom for left and right values
export const CardiacOutputAtom = atom((get) => {
  const leftHeart = get(leftHeartAtom);
  const rightHeart = get(rightHeartAtom);
  return {
    left: leftHeart?.CardiacOutput,
    right: rightHeart?.CardiacOutput
  };
});

// Default visibility settings for subset view
export const pressureCardVisibilityAtom = atom({
  MAP: true,
  PAP: true,
  CVP: true,
  AOP: true,
  LAP: true,
  RAP: true,
  IVC: true,
  RCO: true,
  LCO: true
});

// Helper function to convert number pressure reading to string format
function convertPressureReading(reading: { MaxValue: number, MinValue: number, MeanValue: number }): PressureSensor {
  // Remove quotes and ensure we have clean numbers
  const maxValue = typeof reading.MaxValue === 'string' ? parseFloat(reading.MaxValue) : reading.MaxValue;
  const minValue = typeof reading.MinValue === 'string' ? parseFloat(reading.MinValue) : reading.MinValue;
  const meanValue = typeof reading.MeanValue === 'string' ? parseFloat(reading.MeanValue) : reading.MeanValue;
  
  return {
    MaxValue: maxValue.toString(),
    MinValue: minValue.toString(),
    MeanValue: meanValue.toString()
  };
}

// Message handler setup
export function initializeMessageHandling(setAtoms: {
  setSensorReadings: (value: SensorReadings) => void;
  setLeftHeart: (value: HeartMetrics | null) => void;
  setRightHeart: (value: HeartMetrics | null) => void;
  setStopwatch: (value: number) => void;
}) {
  console.log("Initializing message handling in child window");
  
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
        setAtoms.setSensorReadings(convertedSensors);
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
        if (type === 'UPDATE_LEFT_HEART') {
          setAtoms.setLeftHeart(heartMetrics);
        } else {
          setAtoms.setRightHeart(heartMetrics);
        }
        break;
      case 'UPDATE_STOPWATCH':
        setAtoms.setStopwatch(data);
        break;
    }
  });
}