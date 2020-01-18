load('api_config.js');
load('api_events.js');
load('api_log.js');
load('api_gpio.js');
load('api_timer.js');
load('api_sys.js');

let btn = Cfg.get('board.btn1.pin');              // Built-in button GPIO
let led = Cfg.get('board.led1.pin');              // Built-in LED GPIO number
let onhi = Cfg.get('board.led1.active_high');     // LED on when high?
let state = {on: false, btnCount: 0, uptime: 0};  // Device state
let online = false;                               // Connected to the cloud?


let TpadEvent = {
  TOUCH9: Event.baseNumber('TPE'),  // 'TPE' match 'TPAD_EVT_BASE' in main.c
  LONG_TOUCH9: Event.baseNumber('TPE') + 1,
  UNTOUCH9: Event.baseNumber('TPE') + 2,
  DOUBLE_TOUCH9: Event.baseNumber('TPE') + 3,
};

let setLED = function(on) {
  let level = onhi ? on : !on;
  GPIO.write(led, level);
  print('LED on ->', on);
};

GPIO.set_mode(led, GPIO.MODE_OUTPUT);
setLED(state.on);

let reportState = function() {
  Shadow.update(0, state);
};

// Update state every second, and report to cloud if online
Timer.set(5000, Timer.REPEAT, function() {
  state.uptime = Sys.uptime();
  state.ram_free = Sys.free_ram();
  Log.print(Log.INFO, 'state: ' +JSON.stringify(state));
  // print('online:', online, JSON.stringify(state));
}, null);

Event.addHandler(TpadEvent.TOUCH9, function(ev, evdata, ud) {
  Log.print(Log.INFO, 'handling TOUCH9');
}, null );

Event.addHandler(TpadEvent.LONG_TOUCH9, function(ev, evdata, ud) {
  Log.print(Log.INFO, 'handling LONG_TOUCH9');
}, null );

Event.addHandler(TpadEvent.UNTOUCH9, function(ev, evdata, ud) {
  Log.print(Log.INFO, 'handling UNTOUCH9');
}, null );

Event.addHandler(TpadEvent.DOUBLE_TOUCH9, function(ev, evdata, ud) {
  Log.print(Log.INFO, 'handling DOUBLE_TOUCH9');
}, null );
