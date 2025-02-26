import React, { useState, useCallback, useMemo } from 'react';
import { Button } from '@/components/ui/button.tsx';
import { Minus, Plus, X } from "lucide-react";
import { useAtom } from 'jotai';
import { metricAlarmSettingsAtom } from '@/lib/datastore.ts';

interface AlarmControlProps {
    title?: string;
    subtitle?: string;
    initialValues?: {
        minWarning: number;
        minError: number;
        maxWarning: number;
        maxError: number;
    };
    onClose?: () => void;
    metricId?: string;
}

interface NumberControlProps {
    label: string;
    value: number;
    onMinus: () => void;
    onPlus: () => void;
    subtitle: string;
}

const NumberControl = React.memo(({ label, value, onMinus, onPlus, subtitle }: NumberControlProps) => (
    <div className="flex items-center justify-between mb-4">
        <span className="text-white w-20">{subtitle}</span>
        <div className="flex items-center gap-2">
            <Button
                variant="outline"
                size="icon"
                onClick={onMinus}
                className="h-8 w-8 bg-neutral-700 border-neutral-600"
            >
                <Minus className="h-4 w-4" />
            </Button>
            <span className="text-white w-12 text-center">{value}</span>
            <Button
                variant="outline"
                size="icon"
                onClick={onPlus}
                className="h-8 w-8 bg-neutral-700 border-neutral-600"
            >
                <Plus className="h-4 w-4" />
            </Button>
        </div>
    </div>
));

NumberControl.displayName = 'NumberControl';

const AlarmControl: React.FC<AlarmControlProps> = ({
    title = "SET ALARMS FOR",
    subtitle = "ARTERIAL PRESSURE",
    initialValues = {
        minWarning: 25,
        minError: 30,
        maxWarning: 52,
        maxError: 67
    },
    onClose,
    metricId
}) => {
    type AlarmKey = 'minWarning' | 'minError' | 'maxWarning' | 'maxError';
    
    interface AlarmValues {
        minWarning: number;
        minError: number;
        maxWarning: number;
        maxError: number;
    }

    const [values, setValues] = useState<AlarmValues>(initialValues);
    const [alarmSettings, setAlarmSettings] = useAtom(metricAlarmSettingsAtom);

    const handleChange = useCallback((key: AlarmKey, increment: number): void => {
        setValues(prev => ({
            ...prev,
            [key]: prev[key] + increment
        }));
    }, []);

    const handleSave = useCallback(() => {
        if (metricId) {
            setAlarmSettings(prev => ({
                ...prev,
                [metricId]: values
            }));
        }
        onClose?.();
    }, [metricId, values, setAlarmSettings, onClose]);

    // Calculate positions for slider visualization (0-100%)
    const getPosition = useCallback((value: number): number => {
        const min = Math.min(values.minWarning, values.minError);
        const max = Math.max(values.maxWarning, values.maxError);
        const range = max - min;
        return ((value - min) / range) * 100;
    }, [values]);

    const sliderVisualization = useMemo(() => (
        <div className="mb-8">
            <div className="flex justify-between mb-2">
                <span className="text-white text-sm">Min</span>
                <span className="text-white text-sm">Max</span>
            </div>

            <div className="relative h-1 bg-gray-700 mb-2">
                {/* Warning markers */}
                <div
                    className="absolute w-1 h-3 bg-neutral-400 -top-1"
                    style={{ left: `${getPosition(values.minWarning)}%` }}
                />
                <div
                    className="absolute w-1 h-3 bg-neutral-400 -top-1"
                    style={{ left: `${getPosition(values.maxWarning)}%` }}
                />
                {/* Error markers */}
                <div
                    className="absolute w-1 h-3 bg-neutral-400 -top-1"
                    style={{ left: `${getPosition(values.minError)}%` }}
                />
                <div
                    className="absolute w-1 h-3 bg-neutral-400 -top-1"
                    style={{ left: `${getPosition(values.maxError)}%` }}
                />
            </div>
            <div className="flex justify-between text-white text-sm">
                <div className="flex gap-4">
                    <span>{values.minWarning}</span>
                    <span>{values.minError}</span>
                </div>
                <div className="flex gap-4">
                    <span>{values.maxWarning}</span>
                    <span>{values.maxError}</span>
                </div>
            </div>
        </div>
    ), [values, getPosition]);

    return (
        <div className="bg-neutral-800 p-6 rounded-lg w-64">
            {/* Header */}
            <div className="mb-6">
                <div className="flex justify-between items-center">
                    <div>
                        <h2 className="text-center text-white text-sm mb-1">{title}</h2>
                        <h3 className="text-center text-white text-sm">{subtitle}</h3>
                    </div>
                    {onClose && (
                        <button 
                            onClick={onClose}
                            className="text-gray-400 hover:text-neutral-200"
                        >
                            <X size={20} />
                        </button>
                    )}
                </div>
            </div>

            {sliderVisualization}

            {/* Min Controls */}
            <div className="mb-6">
                <h4 className="text-white mb-4">Min</h4>
                <NumberControl
                    label="Warning"
                    value={values.minWarning}
                    onMinus={() => handleChange('minWarning', -1)}
                    onPlus={() => handleChange('minWarning', 1)}
                    subtitle="Warning"
                />
                <NumberControl
                    label="Error"
                    value={values.minError}
                    onMinus={() => handleChange('minError', -1)}
                    onPlus={() => handleChange('minError', 1)}
                    subtitle="Error"
                />
            </div>

            {/* Max Controls */}
            <div className="mb-6">
                <h4 className="text-white mb-4">Max</h4>
                <NumberControl
                    label="Warning"
                    value={values.maxWarning}
                    onMinus={() => handleChange('maxWarning', -1)}
                    onPlus={() => handleChange('maxWarning', 1)}
                    subtitle="Warning"
                />
                <NumberControl
                    label="Error"
                    value={values.maxError}
                    onMinus={() => handleChange('maxError', -1)}
                    onPlus={() => handleChange('maxError', 1)}
                    subtitle="Error"
                />
            </div>

            {/* Save Button */}
            <Button 
                className="w-full bg-blue-600 hover:bg-blue-700 text-white"
                onClick={handleSave}
            >
                Save Changes
            </Button>
        </div>
    );
};

export default React.memo(AlarmControl);