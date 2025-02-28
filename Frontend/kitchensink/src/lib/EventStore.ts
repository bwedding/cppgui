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
    if (window.chrome?.webview) 
    {
      // Initialize form capture for all forms on the page
      initFormCapture();

      // Set up listener for messages from the host
      window.chrome.webview.addEventListener('message', (event) => {
        // Handle incoming messages if needed
        const message = event.data;
        // Process message...
      });
      document.addEventListener('submit', (event) => {
        if ((event.target as HTMLElement)?.tagName === 'FORM') {
            event.preventDefault(); // Prevent the default form submission
            const formData: { [key: string]: string } = {};
            const formElements = (event.target as HTMLFormElement).elements;
            for (let element of formElements) {
                const inputElement = element as HTMLInputElement;
                if (inputElement.name) {
                    formData[inputElement.name] = inputElement.value;
                }
            }
            const messageString = JSON.stringify(formData);
            // Send the serialized JSON data to the backend
            const result = window.chrome.webview.hostObjects.sync.native.SendForm(messageString);
            console.log('Backend response:', result);
        };
      });
    } else {
      console.warn('WebView2 not available');
    }
  }
}));

// Initialize form capture utility
function initFormCapture(): void {
  document.addEventListener('submit', (event: Event) => {

    if ((event.target as HTMLElement)?.tagName === 'FORM') 
    {
      const form = event.target as HTMLFormElement;
      
      // Skip forms with data-no-capture attribute
      if (!form.hasAttribute('data-no-capture')) 
      {
        event.preventDefault();
        
        const formData: { [key: string]: string | string[] | boolean } = {};
        const formElements = form.elements;
        
        for (let i = 0; i < formElements.length; i++) 
        {
          const element = formElements[i] as HTMLInputElement | HTMLSelectElement | HTMLTextAreaElement;
          
          if (!element.name) continue;
          
          // Handle different input types
          if (element.type === 'checkbox' || element.type === 'radio') 
          {
            const inputElement = element as HTMLInputElement;
            if (inputElement.checked) {
              formData[element.name] = inputElement.value;
            }
          } 
          else if (element.type === 'select-multiple') 
          {
            const selectElement = element as HTMLSelectElement;
            formData[element.name] = Array.from(selectElement.options)
              .filter(option => option.selected)
              .map(option => option.value);
          } 
          else if (element.name) 
          {
            formData[element.name] = element.value;
          }
        }
        
        const messageString = JSON.stringify(formData);
        
        try {
          // Send the serialized JSON data to the backend
          const result = window.chrome.webview.hostObjects.sync.native.SendForm(messageString);
          console.log('Form captured successfully:', result);
          
          // Optional: Allow the form to continue normal submission after capture
          // form.submit();
        } catch (error) {
          console.error('Error capturing form data:', error);
        }
      }
    }
  });
}


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