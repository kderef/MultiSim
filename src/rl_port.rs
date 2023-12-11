pub fn get_current_monitor() -> i32 {
    unsafe {
        raylib::ffi::GetCurrentMonitor()
    }
}

pub fn get_monitor_refresh_rate(monitor: i32) -> i32 {
    unsafe {
        raylib::ffi::GetMonitorRefreshRate(monitor)
    }
}