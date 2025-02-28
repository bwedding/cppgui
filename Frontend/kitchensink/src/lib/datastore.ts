// store.ts
import { atom, getDefaultStore } from 'jotai';
import { HeartMonitorData } from '../types/types.ts';
import { WebView2WebMessageReceivedEventArgs } from '../types/window.ts';
import { atomWithStorage } from 'jotai/utils';
import { HistoryData } from '../types/history.ts';
import { signal, computed } from "@preact/signals-react";

// Initialize the default store
const store = getDefaultStore();
export const heartMonitorSignal = signal<HeartMonitorData | null>(null);

// Derived signals for specific sections 
export const timestampSignal = computed(() => heartMonitorSignal.value?.Timestamp);
export const systemIdSignal = computed(() => heartMonitorSignal.value?.SystemId);

// Status Data signals
export const statusDataSignal = computed(() => heartMonitorSignal.value?.StatusData);
export const canStatusSignal = computed(() => statusDataSignal.value?.CANStatus);
export const strokesSignal = computed(() => statusDataSignal.value?.Strokes);

// Left/Right Heart signals
export const leftHeartSignal = computed(() => heartMonitorSignal.value?.LeftHeart);
export const rightHeartSignal = computed(() => heartMonitorSignal.value?.RightHeart);

// Sensors signals
export const sensorsSignal = computed(() => heartMonitorSignal.value?.Sensors);
export const artPressSignal = computed(() => sensorsSignal.value?.artPress);
export const papSignal = computed(() => sensorsSignal.value?.pap);
export const cvpSignal = computed(() => sensorsSignal.value?.cvp);
export const aopSignal = computed(() => sensorsSignal.value?.aop);
export const ivcSignal = computed(() => sensorsSignal.value?.ivc);

// Heart Rate signal
export const heartRateSignal = computed(() => heartMonitorSignal.value?.HeartRate);

// Operation State signal
export const operationStateSignal = computed(() => heartMonitorSignal.value?.OperationState);

// Heart Status signal
export const heartStatusSignal = computed(() => heartMonitorSignal.value?.HeartStatus);

// Flow Limit signal
export const flowLimitSignal = computed(() => heartMonitorSignal.value?.FlowLimit);

// Root atom that holds the complete state - made writable for internal updates
export const heartMonitorAtom = atom<HeartMonitorData | null, [HeartMonitorData], void>(
    null,
    (_get, set, update) => set(heartMonitorAtom, update)
);

function updateHeartMonitor(update: HeartMonitorData) {
  //console.log('Updating heartMonitor Signal data:', update);
  heartMonitorSignal.value = update;
  console.log('Heartrate in heartMonitor Signal data:', heartMonitorSignal.value);
  
  // Update jotai store as well during transition period
  store.set(heartMonitorAtom, update);
  
  // If we have the history signals imported, we could update them here
  // This will need to be moved to a separate function once we import the history signals
  // to avoid circular dependencies
}

// Create a separate function to update history signals
// This will be called after initializing the signals
export function setupHistorySignalUpdates() 
{
  // We'll implement this later after importing history signals
}

// Alarm Settings
export interface AlarmSettings {
  minWarning: number;
  minError: number;
  maxWarning: number;
  maxError: number;
}

export interface MetricAlarmSettings {
  [metricId: string]: AlarmSettings;
}

export const metricAlarmSettingsAtom = atomWithStorage<MetricAlarmSettings>('metricAlarmSettings', {
  MAP: {
    minWarning: 25,
    minError: 30,
    maxWarning: 52,
    maxError: 67
  }
});

// Pressure Card Visibility Settings
export interface PressureCardVisibility {
  [metricId: string]: boolean;
}

export const pressureCardVisibilitySignal = computed(() => store.get(pressureCardVisibilityAtom));
export const pressureCardVisibilityAtom = atomWithStorage<PressureCardVisibility>('pressureCardVisibility', {
  MAP: true,
  PAP: true,
  CVP: true,
  AOP: true,
  IVC: true,
  LH: true,
  RH: true,
  LAP: true,
  RAP: true,
  LCO: true,
  RCO: true
});

// Create history atoms for each metric
export const leftOutflowPressureHistoryAtom = atomWithStorage<HistoryData>('leftOutflowPressureHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightOutflowPressureHistoryAtom = atomWithStorage<HistoryData>('rightOutflowPressureHistory', {
    times: [],
    oscillations: [],
    mean: null
  });
  
export const leftThermistorTempHistoryAtom = atomWithStorage<HistoryData>('leftThermistorTempHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightThermistorTempHistoryAtom = atomWithStorage<HistoryData>('rightThermistorTempHistory', {
    times: [],
    oscillations: [],
    mean: null
  });
  
export const leftSensorTempHistoryAtom = atomWithStorage<HistoryData>('leftSensorTempHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightSensorTempHistoryAtom = atomWithStorage<HistoryData>('rightSensorTempHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const leftStrokeVolumeHistoryAtom = atomWithStorage<HistoryData>('leftStrokeVolumeHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightStrokeVolumeHistoryAtom = atomWithStorage<HistoryData>('rightStrokeVolumeHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const leftStrokeLengthHistoryAtom = atomWithStorage<HistoryData>('leftStrokeLengthHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightStrokeLengthHistoryAtom = atomWithStorage<HistoryData>('rightStrokeLengthHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const leftCardiacOutputHistoryAtom = atomWithStorage<HistoryData>('leftCardiacOutputHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightCardiacOutputHistoryAtom = atomWithStorage<HistoryData>('rightCardiacOutputHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const leftPowerConsumptionHistoryAtom = atomWithStorage<HistoryData>('leftPowerConsumptionHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

export const rightPowerConsumptionHistoryAtom = atomWithStorage<HistoryData>('rightPowerConsumptionHistory', {
    times: [],
    oscillations: [],
    mean: null
  });

const MAX_HISTORY_MS = 30 * 60 * 1000; // 30 minutes in milliseconds

// Initialize with empty history structure
const initialHistory: HistoryData = {
  times: [],
  oscillations: [],
  mean: null
};

export const leftHeartHistoryAtom = atomWithStorage<HistoryData>('leftHeartHistory', initialHistory);
export const rightHeartHistoryAtom = atomWithStorage<HistoryData>('rightHeartHistory', initialHistory);

// store.ts
export const initializeLogging = () => {
    console.log('Setting up logging subscription...');

    
    // Add an immediate check of the atom's value
    console.log('Current atom value:', store.get(heartMonitorAtom));
    
    store.sub(heartMonitorAtom, () => {
        console.log('Subscription triggered!');
        const data = store.get(heartMonitorAtom);
        if (!data) {
            console.log('No data in atom');
            return;
        }

        // Simple log first to verify we're getting here
        console.log('Raw data received in subscription:', data);

        // Then the structured logging
        console.group('Heart Monitor Update');
        
        // System Info
        console.log('System:', {
            id: data.SystemId,
            time: data.LocalClock,
            timestamp: data.Timestamp
        });

        // Key Metrics
        console.log('Key Metrics:', {
            heartRate: data.HeartRate,
            status: data.HeartStatus,
            operationState: data.OperationState
        });

        // Pressure Data
        console.log('Left Heart Pressure Data:', {
            intPressure: data.LeftHeart?.IntPressure,
            intPressureMin: data.LeftHeart?.IntPressureMin,
            intPressureMax: data.LeftHeart?.IntPressureMax
        });

        console.log('Right Heart Pressure Data:', {
            intPressure: data.RightHeart?.IntPressure,
            intPressureMin: data.RightHeart?.IntPressureMin,
            intPressureMax: data.RightHeart?.IntPressureMax
        });

        console.groupEnd();
    });
    console.log('Logging subscription set up complete');
};

const isHeartMonitorData = (data: unknown): data is HeartMonitorData => {
    if (!data || typeof data !== 'object' || data === null) {
        return false;
    }

    // Cast to any to check properties
    const d = data as any;
    
    // Check required properties and their types
    return (
        typeof d.SystemId === 'string' &&
        typeof d.HeartStatus === 'string' &&
        typeof d.LeftHeart === 'object' && d.LeftHeart !== null &&
        typeof d.RightHeart === 'object' && d.RightHeart !== null &&
        typeof d.StatusData === 'object' && d.StatusData !== null
    );
};

export const apiClient = {
    
    async get(endpoint: string, postData: any = null) {
        const requestPayload = JSON.stringify({
            endpoint: endpoint,
            postData: postData
        });

        console.log('API called with endpoint:', endpoint);
        try {
            if (!window.chrome?.webview?.hostObjects?.bridge) {
                console.error('WebView2 bridge is not available');
                throw new Error('WebView2 bridge is not available');
            }
            const response = await window.chrome.webview.hostObjects.bridge.getData(requestPayload);

            return JSON.parse(response);
        } catch (error) {
            console.error('API call failed:', error);
            throw error;
        }
    },

    async post(endpoint: string, postData: any) {
        return this.get(endpoint, postData);
    },

    async copyToClipboard(text: string) {
        const requestPayload = JSON.stringify({
            endpoint: "/api/clipboard/copy",
            postData: { text: text }
        });
        await window.chrome.webview.hostObjects.bridge.getData(requestPayload);
        console.log("Text copied to clipboard");
    },

    async saveFileDialog() {
        const requestPayload = JSON.stringify({
            endpoint: "/api/saveFileDialog",
            postData: null // No post data needed for this request
        });
    
        console.log('API called to open save file dialog');
        try {
            if (!window.chrome?.webview?.hostObjects?.bridge) {
                console.error('WebView2 bridge is not available');
                throw new Error('WebView2 bridge is not available');
            }
            const response = await window.chrome.webview.hostObjects.bridge.getData(requestPayload);
    
            // Parse the response to get the file path
            const filePath = JSON.parse(response);
            console.log('Selected file path:', filePath);
            return filePath;
        } catch (error) {
            console.error('API call failed:', error);
            throw error;
        }
    },

    async browseForFolder() {
        const requestPayload = JSON.stringify({
            endpoint: "/api/browseForFolder",
            postData: null // No post data needed for this request
        });
    
        console.log('API called to browse for folder');
        try {
            if (!window.chrome?.webview?.hostObjects?.bridge) {
                console.error('WebView2 bridge is not available');
                throw new Error('WebView2 bridge is not available');
            }
            const response = await window.chrome.webview.hostObjects.bridge.getData(requestPayload);
    
            // Parse the response to get the folder path
            const folderPath = JSON.parse(response);
            console.log('Selected folder path:', folderPath);
            return folderPath;
        } catch (error) {
            console.error('API call failed:', error);
            throw error;
        }
    },

    async getLoggedInUserInfo() {
        const requestPayload = JSON.stringify({
            endpoint: "/api/loggedInUserInfo",
            postData: null
        });
    
        console.log('API called to get logged-in user info');
        try {
            if (!window.chrome?.webview?.hostObjects?.bridge) {
                console.error('WebView2 bridge is not available');
                throw new Error('WebView2 bridge is not available');
            }
            const response = await window.chrome.webview.hostObjects.bridge.getData(requestPayload);
    
            // Parse the response to get the user info
            const userInfo = JSON.parse(response);
            console.log('Logged in user info:', userInfo);
            return userInfo;
        } catch (error) {
            console.error('API call failed:', error);
            throw error;
        }
    },

    // openAndReadFile('fileContent');
    async openAndReadFile(divName : string) {
        const requestPayload = JSON.stringify({
            endpoint: "/api/openFileDialog",
            postData: { divName: divName }
        });
    
        console.log('API called to open and read file');
        try {
            if (!window.chrome?.webview?.hostObjects?.bridge) {
                console.error('WebView2 bridge is not available');
                throw new Error('WebView2 bridge is not available');
            }
            const response = await window.chrome.webview.hostObjects.bridge.getData(requestPayload);
    
            // Parse the response to get the file content
            const fileData = JSON.parse(response);
            console.log('File Path:', fileData.filePath);
            console.log('File Content:', fileData.fileContent);
    
            // Display file content in the specified HTML element
            const element = document.getElementById(divName);
            if (!element) {
                throw new Error(`Element with id '${divName}' not found`);
            }
            element.innerText = fileData.fileContent;
        } catch (error) {
            console.error('API call failed:', error);
            throw error;
        }
    }
};
  
  export const initializeWebView2Listener = () => {
    //initializeLogging();
    console.log('Starting WebView2 listener initialization...');
    const store = getDefaultStore();
    
    store.sub(heartMonitorAtom, () => {
        const data = store.get(heartMonitorAtom);
        if (!data) return;
    });
    
    if (window.chrome?.webview) {
        window.chrome.webview.addEventListener('message', (event: WebView2WebMessageReceivedEventArgs) => {
            let data: unknown;
            
            // Parse if string
            if (typeof event.data === 'string') {
                try {
                    data = JSON.parse(event.data);
                } catch (e) {
                    console.error('Failed to parse message data:', e);
                    return;
                }
            } else {
                data = event.data;
            }

            // Validate and update
            if (isHeartMonitorData(data)) {
                store.set(heartMonitorAtom, data);
                updateHeartMonitor(data);
                //console.log('Heart monitor data updated');
            }
        });

        console.log('WebView2 listener initialized');
    }
};

// Derived atoms for specific sections
export const timestampAtom = atom((get) => get(heartMonitorAtom)?.Timestamp);
export const systemIdAtom = atom((get) => get(heartMonitorAtom)?.SystemId);

// Stopwatch atom
export const stopwatchAtom = atom<number>(0);

// Window update functions
interface PressureReading {
  MaxValue: number;
  MinValue: number;
  MeanValue: number;
}

interface SensorReadings {
  artPress?: PressureReading;
  pap?: PressureReading;
  cvp?: PressureReading;
  aop?: PressureReading;
  ivc?: PressureReading;
}

interface HeartReading {
  IntPressure?: PressureReading;
}

// Helper function to convert string pressure sensor to number pressure reading
function convertPressureSensor(sensor: { MaxValue: string; MinValue: string; MeanValue: string; } | undefined): PressureReading | undefined {
  if (!sensor) return undefined;
  return {
    MaxValue: Number(sensor.MaxValue),
    MinValue: Number(sensor.MinValue),
    MeanValue: Number(sensor.MeanValue)
  };
}

// Function to send updated sensor readings
export function updateChildWindowSensors(childWindow: Window, readings: { 
  aop?: { MaxValue: string; MinValue: string; MeanValue: string; };
  cvp?: { MaxValue: string; MinValue: string; MeanValue: string; };
  pap?: { MaxValue: string; MinValue: string; MeanValue: string; };
  artPress?: { MaxValue: string; MinValue: string; MeanValue: string; };
  ivc?: { MaxValue: string; MinValue: string; MeanValue: string; };
} | null) {
  if (!readings) {
    console.log('updateChildWindowSensors: No readings provided');
    return;
  }

  if (!childWindow || childWindow.closed) {
    console.error('updateChildWindowSensors: Child window is null or closed');
    return;
  }

  try {
    console.log('updateChildWindowSensors: Converting sensor readings', readings);
    
    const sensorData: SensorReadings = {
      artPress: convertPressureSensor(readings.artPress),
      pap: convertPressureSensor(readings.pap),
      cvp: convertPressureSensor(readings.cvp),
      aop: convertPressureSensor(readings.aop),
      ivc: convertPressureSensor(readings.ivc)
    };

    // Validate converted data
    if (!sensorData.artPress && !sensorData.pap && !sensorData.cvp && !sensorData.aop && !sensorData.ivc) {
      console.warn('updateChildWindowSensors: All sensor readings are null or invalid');
      return;
    }

    console.log('updateChildWindowSensors: Sending message with data', sensorData);
    
    childWindow.postMessage({
      type: 'UPDATE_SENSORS',
      data: sensorData
    }, window.location.origin);
  } catch (error) {
    console.error('updateChildWindowSensors: Error updating sensors:', error);
  }
}

// Function to update heart readings
export function updateChildWindowHearts(
  childWindow: Window, 
  leftHeartData: {
    StrokeVolume: string;
    PowerConsumption: { MaxValue: string; MinValue: string; MeanValue: string; };
    IntPressure: { MaxValue: string; MinValue: string; MeanValue: string; };
    MedicalPressure: { MaxValue: string; MinValue: string; MeanValue: string; };
    IntPressureMin: number;
    IntPressureMax: number;
    CardiacOutput: { MaxValue: string; MinValue: string; MeanValue: string; };
    ActualStrokeLen: string;
    TargetStrokeLen: string;
    SensorTemperature: string;
    ThermistorTemperature: string;
    CpuLoad: string;
    OutflowPressure: string;
  } | null, 
  rightHeartData: {
    StrokeVolume: string;
    PowerConsumption: { MaxValue: string; MinValue: string; MeanValue: string; };
    IntPressure: { MaxValue: string; MinValue: string; MeanValue: string; };
    MedicalPressure: { MaxValue: string; MinValue: string; MeanValue: string; };
    IntPressureMin: number;
    IntPressureMax: number;
    CardiacOutput: { MaxValue: string; MinValue: string; MeanValue: string; };
    ActualStrokeLen: string;
    TargetStrokeLen: string;
    SensorTemperature: string;
    ThermistorTemperature: string;
    CpuLoad: string;
    OutflowPressure: string;
  } | null
) {
  if (!leftHeartData && !rightHeartData) {
    console.log('updateChildWindowHearts: Missing heart data', { leftHeartData, rightHeartData });
    return;
  }

  if (!childWindow || childWindow.closed) {
    console.error('updateChildWindowHearts: Child window is null or closed');
    return;
  }

  try {
    console.log('updateChildWindowHearts: Converting heart data', { leftHeartData, rightHeartData });

    const leftHeart: HeartReading = {
      IntPressure: leftHeartData ? convertPressureSensor(leftHeartData.IntPressure) : undefined
    };

    const rightHeart: HeartReading = {
      IntPressure: rightHeartData ? convertPressureSensor(rightHeartData.IntPressure) : undefined
    };

    // Validate converted data
    if (!leftHeart.IntPressure && !rightHeart.IntPressure) {
      console.warn('updateChildWindowHearts: Both heart readings are null or invalid');
      return;
    }

    if (leftHeart.IntPressure) {
      console.log('updateChildWindowHearts: Sending left heart data', leftHeart);
      childWindow.postMessage({
        type: 'UPDATE_LEFT_HEART',
        data: leftHeart
      }, window.location.origin);
    }
    
    if (rightHeart.IntPressure) {
      console.log('updateChildWindowHearts: Sending right heart data', rightHeart);
      childWindow.postMessage({
        type: 'UPDATE_RIGHT_HEART',
        data: rightHeart
      }, window.location.origin);
    }
  } catch (error) {
    console.error('updateChildWindowHearts: Error updating hearts:', error);
  }
}

// Function to update stopwatch
export function updateChildWindowStopwatch(childWindow: Window, time: number) {
  childWindow.postMessage({
    type: 'UPDATE_STOPWATCH',
    data: time
  }, window.location.origin);
}

// Status Data atoms
export const statusDataAtom = atom((get) => get(heartMonitorAtom)?.StatusData);
export const canStatusAtom = atom((get) => get(statusDataAtom)?.CANStatus);
export const strokesAtom = atom((get) => get(statusDataAtom)?.Strokes);
export const busLoadAtom = atom((get) => get(statusDataAtom)?.BusLoad);

// Heart Data atoms
export const leftHeartAtom = atom((get) => get(heartMonitorAtom)?.LeftHeart);
export const rightHeartAtom = atom((get) => get(heartMonitorAtom)?.RightHeart);

// Derived heart metrics
export const heartMetricsAtom = atom((get) => ({
    heartRate: get(heartMonitorAtom)?.HeartRate,
    operationState: get(heartMonitorAtom)?.OperationState,
    heartStatus: get(heartMonitorAtom)?.HeartStatus,
    flowLimit: get(heartMonitorAtom)?.FlowLimit
}));

// Sensor readings signal
export const sensorReadingsSignal = computed(() => ({
    aop: heartMonitorSignal.value?.AoPSensor,
    cvp: heartMonitorSignal.value?.CVPSensor,
    pap: heartMonitorSignal.value?.PAPSensor,
    artPress: heartMonitorSignal.value?.ArtPressSensor,
    ivc: heartMonitorSignal.value?.IVCSensorVal
}));

// Atrial Pressures signal for left and right pressure values
export const atrialPressuresSignal = computed(() => {
  return {
    left: leftHeartSignal.value?.IntPressure,
    right: rightHeartSignal.value?.IntPressure
  };
});

// Cardiac Output signal for left and right values
export const cardiacOutputSignal = computed(() => {
  return {
    left: leftHeartSignal.value?.CardiacOutput,
    right: rightHeartSignal.value?.CardiacOutput
  };
});

// Atrial Pressures atom for left and right pressure values
export const AtrialPressuresAtom = atom((get) => {
  const heartMonitor = get(heartMonitorAtom);
  return {
    left: heartMonitor?.LeftHeart?.IntPressure,
    right: heartMonitor?.RightHeart?.IntPressure
  };
});

// Cardiac Output atom for left and right values
export const CardiacOutputAtom = atom((get) => {
  const heartMonitor = get(heartMonitorAtom);
  return {
    left: heartMonitor?.LeftHeart?.CardiacOutput,
    right: heartMonitor?.RightHeart?.CardiacOutput
  };
});

export { store };
