import { atomWithStorage } from 'jotai/utils';

// Initialize atoms with default values
export const isRunningAtom = atomWithStorage('stopwatchRunning', false);
export const elapsedTimeAtom = atomWithStorage('stopwatchElapsedTime', 0);
