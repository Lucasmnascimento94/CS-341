################################################################################################################
#  Check dependencies on Linux Environment  
function check_dependencies{ 
    param (
        [switch]$verbose
    )
    
    $DEP = (Get-Location).Path + "/dependencies/dependencies.txt" # txt File which holds necessary packages for the program to run

    if ($verbose){
        foreach ($line in Get-Content -Path $DEP) {
            if(Get-Package -Name $line -ErrorAction SilentlyContinue){
                Write-Output "Dependencie Already Installed -> $line"
                
            }
            else{
                Write-Output "Processing: $line"
                # ensure this session can see winget (don't break it if PATH has %vars%)
                if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
                    $env:Path = [Environment]::ExpandEnvironmentVariables(
                        [Environment]::GetEnvironmentVariable('Path','Machine') + ';' +
                        [Environment]::GetEnvironmentVariable('Path','User')
                    )
                    $wa = "$env:LOCALAPPDATA\Microsoft\WindowsApps"
                    if (Test-Path "$wa\winget.exe") { $env:Path = "$wa;$env:Path" }
                }

                if (Get-Command winget -ErrorAction SilentlyContinue) {
                    $installed = $false
                    if ($line -match '^[A-Za-z0-9_.-]+\.[A-Za-z0-9_.-]+$') {
                        winget install --id $line -e -h --accept-source-agreements | Out-Null
                        if ($LASTEXITCODE -eq 0) { $installed = $true }
                    }
                    if (-not $installed) {
                        winget install $line -h --accept-source-agreements
                    }
                    $env:Path = [Environment]::ExpandEnvironmentVariables(
                        [Environment]::GetEnvironmentVariable('Path','Machine') + ';' +
                        [Environment]::GetEnvironmentVariable('Path','User')
                    )
                } else {
                    Write-Output ("winget not available - skipping auto-install for '{0}'." -f $line)
                }
                
                $env:Path = [Environment]::ExpandEnvironmentVariables(
                    [Environment]::GetEnvironmentVariable('Path','Machine') + ';' +
                    [Environment]::GetEnvironmentVariable('Path','User')
                )
                $wa = "$env:LOCALAPPDATA\Microsoft\Windows\WindowsApps"
                if (Test-Path "$wa\winget.exe") { $env:Path = "$wa;$env:Path" }
            }
        }
    }
}

check_dependencies -verbose
