// types.ts
export interface StatusIndicator {
    Text: string;
    Color: string;
}

export interface PressureSensor {
    BackColor: string;
    MaxValue: string;
    MeanValue: string;
    MinValue: string;
}

export interface HeartData {
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

export interface SystemStatus {
    ExtLeft: StatusIndicator;
    ExtRight: StatusIndicator;
    CANStatus: StatusIndicator;
    BytesSent: StatusIndicator;
    BytesRecd: StatusIndicator;
    Strokes: StatusIndicator;
    IntLeft: StatusIndicator;
    IntRight: StatusIndicator;
    BusLoad: StatusIndicator;
}

export interface HeartMonitorData {
    Timestamp: string;
    SystemId: string;
    StatusData: SystemStatus;
    LeftHeart: HeartData;
    RightHeart: HeartData;
    HeartRate: string;
    OperationState: string;
    HeartStatus: string;
    FlowLimitState: string;
    FlowLimit: string;
    AtmosPressure: string;
    UseMedicalSensor: boolean;
    AoPSensor: PressureSensor;
    CVPSensor: PressureSensor;
    PAPSensor: PressureSensor;
    ArtPressSensor: PressureSensor;
    IVCSensorVal: PressureSensor;
    LocalClock: string;
    Messages: string[];
}

export type EventType = 
  | 'BUTTON_CLICK'
  | 'MENU_SELECT'
  | 'VALUE_CHANGE'
  | 'PARAMETER_UPDATE'
  | 'LOG_MESSAGE'
  | 'SWITCH_TOGGLE'
  ;

export interface BaseEvent {
  type: EventType;
  timestamp: number;
  source: string;
  metadata?: Record<string, any>;
}

export interface LogMessageEvent extends BaseEvent {
    type: 'LOG_MESSAGE';
    message: string;
    level?: 'info' | 'warning' | 'error';
  }

  export interface ButtonClickEvent extends BaseEvent {
    type: 'BUTTON_CLICK';
    buttonId: string;
    value: boolean;
    metadata: {
      action: string;
      control: string;
      parameter: string;
      currentState: number;
      variant?: string;
      size?: string;
    }
  }

export interface SwitchToggleEvent extends BaseEvent {
  type: 'SWITCH_TOGGLE';
  switchId: string;
  state: string;
}

export interface MenuSelectEvent extends BaseEvent {
  type: 'MENU_SELECT';
  menuId: string;
  selection: string;
  metadata: {
    action: string;
    control: string;
    parameter: string;
    currentState: number;
    variant?: string;
    size?: string;
  }
}

export interface ValueChangeEvent extends BaseEvent {
  type: 'VALUE_CHANGE';
  fieldId: string;
  value: number | string;
  previousValue: number | string;
}

export interface ParameterUpdateEvent extends BaseEvent {
  type: 'PARAMETER_UPDATE';
  parameterId: string;
  value: any;
}

export type ApplicationEvent = 
  | ButtonClickEvent 
  | MenuSelectEvent 
  | ValueChangeEvent 
  | ParameterUpdateEvent
  | LogMessageEvent
  | SwitchToggleEvent
  ;