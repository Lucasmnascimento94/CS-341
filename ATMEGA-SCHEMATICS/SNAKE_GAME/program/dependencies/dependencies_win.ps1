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
                winget install $line
            }
        }

       # if(-not (Get-Package $line)){
       #     Write-Output "Processing nono: $line"
       # }
    }
}

check_dependencies -verbose