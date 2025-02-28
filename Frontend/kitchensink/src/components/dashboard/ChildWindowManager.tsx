import React, { useEffect, useRef, useCallback } from 'react';
import { useAtomValue } from 'jotai';
import { 
  sensorReadingsSignal, 
  leftHeartAtom, 
  rightHeartAtom,
  updateChildWindowSensors,
  updateChildWindowHearts
} from '@/lib/datastore.ts';
import { HeartData } from '@/types/types.ts';
import { useSignals } from '@preact/signals-react/runtime';

interface ChildWindowManagerProps {
  childWindow: Window | null;
  setChildWindow: (window: Window | null) => void;
  stopwatchTime: number;
  setStopwatchTime: (time: number | ((prev: number) => number)) => void;
}

const ChildWindowManager: React.FC<ChildWindowManagerProps> = ({
  childWindow: ORViewWindow,
  setChildWindow: setORViewWindow,
  stopwatchTime,
  setStopwatchTime
}) => {
  // Enable signals reactivity
  useSignals();
  
  // Use signal for sensor readings
  const sensorReadings = sensorReadingsSignal.value;
  const leftHeart = useAtomValue(leftHeartAtom) as HeartData;
  const rightHeart = useAtomValue(rightHeartAtom) as HeartData;

  // Refs to track previous values for change detection
  const prevSensorReadings = useRef(sensorReadings);
  const prevLeftHeart = useRef(leftHeart);
  const prevRightHeart = useRef(rightHeart);
  const isFirstUpdate = useRef(true);

  // Update data when values change
  useEffect(() => {
    if (!ORViewWindow || ORViewWindow.closed) {
      //console.log('ChildWindowManager: Window is closed or null');
      setORViewWindow(null);
      return;
    }

    //console.log('ChildWindowManager: Window is open, checking for updates');

    // Check if window is closed periodically
    const checkWindowInterval = setInterval(() => {
      if (ORViewWindow.closed) {
        //console.log('ChildWindowManager: Window was closed, cleaning up');
        setORViewWindow(null);
        clearInterval(checkWindowInterval);
      }
    }, 1000);

    try {
      // Force initial update when window is first opened
      if (isFirstUpdate.current) {
        //console.log('ChildWindowManager: Performing initial update');
        updateChildWindowSensors(ORViewWindow, sensorReadings);
        updateChildWindowHearts(ORViewWindow, leftHeart, rightHeart);
        isFirstUpdate.current = false;
        prevSensorReadings.current = sensorReadings;
        prevLeftHeart.current = leftHeart;
        prevRightHeart.current = rightHeart;
        return () => clearInterval(checkWindowInterval);
      }

      // Deep compare objects for changes
      const sensorChanged = JSON.stringify(sensorReadings) !== JSON.stringify(prevSensorReadings.current);
      const leftHeartChanged = JSON.stringify(leftHeart) !== JSON.stringify(prevLeftHeart.current);
      const rightHeartChanged = JSON.stringify(rightHeart) !== JSON.stringify(prevRightHeart.current);

      //console.log('ChildWindowManager: Change detection:', {
      //  sensorChanged,
      //  leftHeartChanged,
      //  rightHeartChanged,
      //  currentSensor: sensorReadings,
      //  prevSensor: prevSensorReadings.current,
      //  currentLeft: leftHeart,
      //  prevLeft: prevLeftHeart.current,
      //  currentRight: rightHeart,
      //  prevRight: prevRightHeart.current
      //});

      // Always update if any data has changed
      if (sensorChanged || leftHeartChanged || rightHeartChanged) {
        //console.log('ChildWindowManager: Data changed, sending updates');
        if (sensorChanged) {
          //console.log('ChildWindowManager: Updating sensors');
          updateChildWindowSensors(ORViewWindow, sensorReadings);
          // Create a deep copy of the current readings for comparison
          prevSensorReadings.current = JSON.parse(JSON.stringify(sensorReadings));
        }

        if (leftHeartChanged || rightHeartChanged) {
          //console.log('ChildWindowManager: Updating hearts');
          updateChildWindowHearts(ORViewWindow, leftHeart, rightHeart);
          // Create deep copies of the current heart data for comparison
          prevLeftHeart.current = JSON.parse(JSON.stringify(leftHeart));
          prevRightHeart.current = JSON.parse(JSON.stringify(rightHeart));
        }
      }
    } catch (error) {
      console.error('ChildWindowManager: Error updating child window:', error);
    }

    return () => {
      clearInterval(checkWindowInterval);
    };
  }, [ORViewWindow, sensorReadings, leftHeart, rightHeart, setORViewWindow]);

  // Stopwatch timer effect
  useEffect(() => {
    let interval: NodeJS.Timeout | null = null;
    if (ORViewWindow && !ORViewWindow.closed) {
      //console.log('ChildWindowManager: Starting stopwatch updates');
      interval = setInterval(() => {
        setStopwatchTime(prev => {
          const newTime = prev + 1;
          try {
            if (!ORViewWindow.closed) {
              //console.log('ChildWindowManager: Updating stopwatch:', newTime);
              ORViewWindow.postMessage({
                type: 'UPDATE_STOPWATCH',
                data: newTime
              }, window.location.origin);
            }
          } catch (error) {
            console.error('ChildWindowManager: Error updating stopwatch:', error);
          }
          return newTime;
        });
      }, 1000);
    }
    return () => {
      if (interval) {
        //console.log('ChildWindowManager: Cleaning up stopwatch interval');
        clearInterval(interval);
      }
    };
  }, [ORViewWindow, setStopwatchTime]);

  return null; // This is a logic-only component
};

export default ChildWindowManager;
