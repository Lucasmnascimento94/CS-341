# --------------------------------------------------------------------------------------------------
# SCRIPT-LEVEL PARAM BLOCK: Captures arguments passed to the script file (e.g., -port COM5).
# --------------------------------------------------------------------------------------------------
param(
    [switch]$read, 
    [string]$port, 
    [int]$baud
)

function terminal{
    if($read){
        Write-Host "Reading -  | Port: $port | Baud_Rate: $baud"
        readSerial -port $port -baud $baud
    }
    else{findUsb}
}

function findUsb{
    $filePath= "$PSScriptRoot/usb_devices.txt"
    
    Write-Host "`n=== Searching for USB Serial Ports ===" -ForegroundColor Cyan
    
    # Method 1: Get-PnpDevice (most reliable)
    $devices = Get-PnpDevice -Class Ports | Where-Object Status -eq "OK"
    
    # Method 2: Alternative - Get all COM ports using WMI
    if ($devices.Count -eq 0) {
        Write-Host "Method 1 found no devices. Trying alternative method..." -ForegroundColor Yellow
        $devices = Get-WmiObject Win32_SerialPort | Where-Object { $_.DeviceID -like "COM*" }
    }
    
    # Method 3: List all available COM ports from .NET
    if ($devices.Count -eq 0) {
        Write-Host "Method 2 found no devices. Checking .NET SerialPort..." -ForegroundColor Yellow
        $comPorts = [System.IO.Ports.SerialPort]::GetPortNames()
        if ($comPorts.Count -gt 0) {
            Write-Host "`nFound COM ports via .NET:" -ForegroundColor Green
            foreach ($port in $comPorts) {
                Write-Host "  - $port" -ForegroundColor White
            }
            $comPorts | Out-File "$filePath"
            Write-Host "`nPorts saved to: $filePath" -ForegroundColor Cyan
            return
        }
    }
    
    if ($devices.Count -gt 0) {
        Write-Host "`nFound $($devices.Count) device(s):" -ForegroundColor Green
        $devices | Format-Table -AutoSize | Out-String | Write-Host
        $devices | Out-File "$filePath"
        Write-Host "Devices saved to: $filePath" -ForegroundColor Cyan
    } else {
        Write-Host "`n❌ No USB Serial Ports detected!" -ForegroundColor Red
        Write-Host "`nTroubleshooting steps:" -ForegroundColor Yellow
        Write-Host "1. Check Device Manager (Win+X -> Device Manager)" -ForegroundColor White
        Write-Host "2. Look under 'Ports (COM & LPT)' section" -ForegroundColor White
        Write-Host "3. If device shows with yellow warning:" -ForegroundColor White
        Write-Host "   - Right-click -> Update driver" -ForegroundColor White
        Write-Host "   - Install CP210x or FTDI VCP driver" -ForegroundColor White
        Write-Host "4. If no device appears:" -ForegroundColor White
        Write-Host "   - Unplug and replug USB cable" -ForegroundColor White
        Write-Host "   - Try different USB port" -ForegroundColor White
        Write-Host "   - Check if device is powered on" -ForegroundColor White
        Write-Host "`nManual COM port detection:" -ForegroundColor Cyan
        Write-Host "Run: [System.IO.Ports.SerialPort]::GetPortNames()" -ForegroundColor White
        Write-Host "Or check Device Manager manually." -ForegroundColor White
        
        "No devices found" | Out-File "$filePath"
    }
}

function readSerial{
    param(
        [string]$port,
        [int]$baud
    )
    
    # 1. Instantiate the SerialPort object
    # Using full enum names is good practice:
    $ParityNone = [System.IO.Ports.Parity]::None
    $StopBitsOne = [System.IO.Ports.StopBits]::One

    $SerialPort = New-Object System.IO.Ports.SerialPort "$port", $baud, $ParityNone, 8, $StopBitsOne
    $SerialPort.ReadTimeout = 10 # Set short timeout for continuous polling
    
    # --- START OF CRITICAL RESOURCE MANAGEMENT BLOCK (Main Try/Catch/Finally) ---
    # STRUCTURE IDEA: All serial operations (open/read) are protected. try (work) -> catch (react to error) -> finally (guaranteed port cleanup).
    
    # --- START OF CRITICAL RESOURCE MANAGEMENT BLOCK (Main Try/Catch/Finally) ---
    try {
        # 2. Open the port (Failure here jumps immediately to the outer catch block)
        $SerialPort.Open()
        Write-Host ">> Serial Port $port OPENED. Press Ctrl+C to stop..." -ForegroundColor Green
        
        # 3. Continuous Read Loop
        while($true){
            try {
                $Data = $SerialPort.ReadExisting()
                if (-not [string]::IsNullOrEmpty($Data)){
                    Write-Host $Data -NoNewline # Use -NoNewline for clean output
                }
            }
            # Mandatory: Catch the specific TimeoutException so the loop doesn't crash
            catch [System.TimeoutException]{} 
            
            # Catch block for critical read errors (like device unplugged)
            catch {
                Write-Host "Unexpected Read Error: $($_.Exception.Message)" -ForegroundColor Red
                break # Exit the loop on critical error
            }
            Start-Sleep -Milliseconds 10
        }
    }
    # This CATCH block handles errors from $SerialPort.Open() (like "Access Denied")
    catch {
        Write-Host "`nFATAL ERROR: Could not open port $port." -ForegroundColor Red
        Write-Host "Reason: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "Check if the port is busy (e.g., opened by Arduino IDE)." -ForegroundColor Yellow
    }
    # This FINALLY block runs no matter what, guaranteeing the port closes.
    finally {
        if ($null -ne $SerialPort -and $SerialPort.IsOpen) {
            $SerialPort.Close()
            Write-Host "`n>> Serial Port $port CLOSED <No-Erros>." -ForegroundColor Cyan
        }
    }
    # --- END OF CRITICAL RESOURCE MANAGEMENT BLOCK ---
}

terminal @PSBoundParameters
