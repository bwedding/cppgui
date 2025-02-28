// src/subset.tsx
import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider } from 'jotai';
import { 
  initializeMessageHandling,
  sensorReadingsSignal
} from './lib/subset-datastore.ts';
import SubsetLayout from './components/SubsetLayout.tsx';
import SubsetOperatingRoomView from './components/dashboard/SubsetOperatingRoomView.tsx';
import './index.css';

// Component to initialize message handling
const MessageHandler: React.FC = () => {
  React.useEffect(() => {
    // Initialize message handling with signals
    initializeMessageHandling({ useSignals: true });
  }, []);

  return null;
};

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <Provider>
      <MessageHandler />
      <SubsetLayout>
        <SubsetOperatingRoomView />
      </SubsetLayout>
    </Provider>
  </React.StrictMode>
);