export interface HostObjects {
  sync: {
    native: NativeHost;
  };
  native: NativeHost & {
    SendClick: {
      (item: string): number;  // Sync version
      async(item: string): Promise<number>;  // Async version
    };
  };
  electron: any;
  bridge: {
    getData: (endpoint: string) => Promise<string>;
  };
}

interface NativeHost {
  MaximizeWindow: () => void;
  MinimizeWindow: () => void;
  CloseWindow: () => void;
  StartWindowDrag: () => void;
  FileOpenDialog: string;
  SaveFileDialog: () => string;
  BrowseForFolder: () => string;
  OpenFolderDialog: () => string;
  SendClick: {
    (item: string): number;  // Sync version
    async(item: string): Promise<number>;  // Async version
  };
  SendForm: {
    (item: string): number;  // Sync version
    async(item: string): Promise<number>;  // Async version
  };

}

declare global {
  interface Window {
    chrome: {
      webview: {
        hostObjects: HostObjects;
        postMessage(message: string): void;
        addEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
        removeEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
      };
    };
  }
}

export interface WebView {
  hostObjects: HostObjects;
  postMessage(message: any): void;
}

export interface WebView2WebMessageReceivedEventArgs {
    data: any;
}

export interface WebView2 {
    postMessage(message: any): void;
    addEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
    removeEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
}

export interface Chrome {
    webview: WebView & WebView2 & {
        addEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
        removeEventListener(type: string, listener: (event: WebView2WebMessageReceivedEventArgs) => void): void;
    };
}

declare global {
  interface Window {
    chrome?: Chrome;
  }
}
