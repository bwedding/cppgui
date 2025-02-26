import React, { useState, useCallback, useMemo } from "react";
import { Card } from "@/components/ui/card.tsx";
import AlarmControl from "./AlarmSettings.tsx";
import { useAtom } from "jotai";
import { metricAlarmSettingsAtom } from "@/lib/datastore.ts";
import { createPortal } from "react-dom";

interface AlarmSettings {
  minWarning?: number;
  minError?: number;
  maxWarning?: number;
  maxError?: number;
}

interface PressureCardProps {
  title: string;
  value1: number;
  value2: number;
  mean: number;
  metricId: string;
  visible?: boolean;
}

type CardStatus = 'normal' | 'warning' | 'alert';

// Static style maps to prevent recreation
const STATUS_STYLES = {
  alert: {
    bg: 'bg-red-500',
    text: 'text-black',
    secondaryText: 'text-black'
  },
  warning: {
    bg: 'bg-yellow-600',
    text: 'text-black',
    secondaryText: 'text-black'
  },
  normal: {
    bg: 'bg-neutral-900',
    text: 'text-gray-200',
    secondaryText: 'text-gray-400'
  }
} as const;

const DEFAULT_ALARM_SETTINGS: AlarmSettings = {
};

// Fixed size styles for 30% smaller cards
const RESPONSIVE_CARD_STYLES = {
  textSizeAdjust: 'none' as const,
  width: '100%',
  height: '100%',
};

const PressureCard = React.memo(({
  title,
  value1,
  value2,
  mean,
  metricId,
  visible = true,
}: PressureCardProps) => {
  if (!visible) return null;

  const [showAlarmControl, setShowAlarmControl] = useState(false);
  const [alarmSettings] = useAtom(metricAlarmSettingsAtom);

  const currentAlarmSettings = alarmSettings[metricId] || DEFAULT_ALARM_SETTINGS;
  const maxValue = Math.max(value1, value2);
  const minValue = Math.min(value1, value2);
  
  const status = useMemo<CardStatus>(() => {
    const { minWarning, minError, maxWarning, maxError } = currentAlarmSettings;
    
    // Check error conditions first
    if (minError !== undefined && maxError !== undefined && (maxValue >= maxError || minValue <= minError)) {
      return 'alert';
    }

    // Then check warning conditions
    if (minWarning !== undefined && maxWarning !== undefined && (maxValue >= maxWarning || minValue <= minWarning)) {
      return 'warning';
    }

    return 'normal';
  }, [maxValue, minValue, currentAlarmSettings]);

  // Get styles based on status using static map
  const styles = STATUS_STYLES[status];

  const handleSettingsClick = useCallback((e: React.MouseEvent) => {
    e.stopPropagation(); // Prevent event from bubbling up
    setShowAlarmControl(true);
  }, []);

  const handleClose = useCallback(() => {
    setShowAlarmControl(false);
  }, []);

  const alarmControlPortal = useMemo(() => {
    if (!showAlarmControl) return null;

    return createPortal(
      <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
        <div className="relative" onClick={e => e.stopPropagation()}>
          <AlarmControl
            title="SET ALARMS FOR"
            subtitle={title}
            initialValues={currentAlarmSettings}
            onClose={handleClose}
            metricId={metricId}
          />
        </div>
      </div>,
      document.body
    );
  }, [showAlarmControl, title, metricId, currentAlarmSettings, handleClose]);

  // Memoize formatted values
  const formattedValues = useMemo(() => ({
    value1: value1.toFixed(0),
    value2: value2.toFixed(0),
    mean: mean.toFixed(0)
  }), [value1, value2, mean]);

  return (
    <>
      <Card 
        style={RESPONSIVE_CARD_STYLES}
        className={`${styles.bg} shadow-inner-xl border-gray-800 w-full h-[160px] flex flex-col p-4 relative overflow-hidden`}
      >
        {/* Settings gear icon */}
        <div 
          className="absolute top-3 right-3 p-2 cursor-pointer text-gray-300 hover:text-white z-10"
          onClick={handleSettingsClick}
        >
          <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
            <circle cx="12" cy="12" r="3"></circle>
            <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
          </svg>
        </div>

        <div style={{ textSizeAdjust: 'none' }} className={`text-[54px] font-bold text-center mb-1 ${styles.text} truncate`}>
          {title}
        </div>
        <div style={{ textSizeAdjust: 'none' }} className={`text-[24px] font-bold text-center mb-2 ${styles.text} truncate`}>
          mmHg
        </div>
        <div style={{ textSizeAdjust: 'none' }} className="flex-1 flex flex-col items-center justify-center">
          <div style={{ textSizeAdjust: 'none' }} className="flex items-baseline gap-2 justify-center mb-3 w-full">
            <span className={`text-[70px] leading-none font-bold ${styles.text} truncate`}>
              {formattedValues.value1}
            </span>
            <span className={`text-[70px] leading-none font-light ${styles.text}`}>
              /
            </span>
            <span className={`text-[70px] leading-none font-bold ${styles.text} truncate`}>
              {formattedValues.value2}
            </span>
          </div>
          <div className={`text-[36px] font-bold ${styles.secondaryText} truncate w-full text-center mt-1`}>
            ({formattedValues.mean})
          </div>
        </div>
      </Card>
      {alarmControlPortal}
    </>
  );
});

PressureCard.displayName = 'PressureCard';

export default PressureCard;