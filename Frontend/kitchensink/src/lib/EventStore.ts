import {create} from 'zustand';
import { ApplicationEvent } from '../types/types.ts';

interface EventStore {
  // Actions
  sendEvent: (event: ApplicationEvent) => void;
  initialize: () => void;
  sendLog: (message: string, level?: 'info' | 'warning' | 'error') => void;

}

export const useEventStore = create<EventStore>((set, get) => ({
  sendEvent: async (event: ApplicationEvent) => {
    if (window.chrome?.webview) {
      const startTime = performance.now();

      // Stringify the event object before sending
      const messageString = JSON.stringify(event);
      const jsonTime = performance.now();
      console.log('Sending event from useEventStore:', event);
      // Todo: Check if they are using strings or methods to capture clicks
      //window.chrome.webview.postMessage(messageString);
      try {
        const result = window.chrome.webview.hostObjects.sync.native.SendClick(messageString);
        // Use sync call for long running calls. Button clicks should always use sync. 
        // Async has an overhead of about 250-300ms. Sync version is 2ms or less
        //const result = await window.chrome.webview.hostObjects.native.SendClick(messageString);
        const bridgeTime = performance.now();

        if (result === null) {
          console.log(`Bridge call time: ${bridgeTime - jsonTime}ms`);
          console.log("SendClick sent:", messageString);
        } else {
          console.log(`Bridge call time: ${bridgeTime - jsonTime}ms`);
          console.log("SendClick failed with error code:", result);
        }
      } catch (error) {
        console.error("Error sending click:", error);
      }
    } else {
      console.warn('WebView2 not available');
    }
  },
  sendLog: (message: string, level: 'info' | 'warning' | 'error' = 'info') => {
    if (window.chrome?.webview) {
      get().sendEvent({
        type: 'LOG_MESSAGE',
        timestamp: Date.now(),
        message,
        level,
        source: 'client'
      });
    }
  },

  initialize: () => {
    if (window.chrome?.webview) {
      // Set up listener for messages from the host
      window.chrome.webview.addEventListener('message', (event) => {
        // Handle incoming messages if needed
        const message = event.data;
        // Process message...
      });
    } else {
      console.warn('WebView2 not available');
    }
  }
}));
/*
// Example usage:
const Component = () => {
  const { sendLog } = useEventStore();
  
  const handleClick = () => {
    sendLog('User clicked important button', 'info');
  };

  const handleError = () => {
    sendLog('Failed to save user preferences', 'error');
  };

  // Direct string sending
    if (window.chrome?.webview) {
        window.chrome.webview.postMessage("User clicked the save button");
}
}; */