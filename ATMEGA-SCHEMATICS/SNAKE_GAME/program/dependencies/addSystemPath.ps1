function Add-SystemPath {
  param([Parameter(Mandatory)][string]$dir)

  $mach  = [Environment]::GetEnvironmentVariable('Path','Machine')
  $items = $mach -split ';'
  if ($items -notcontains $dir) {
    $new = ($items + $dir | Select-Object -Unique) -join ';'
    [Environment]::SetEnvironmentVariable('Path', $new, 'Machine')
  }
}

param([Parameter(Mandatory)][string]$dir)

$mach  = [Environment]::GetEnvironmentVariable('Path','Machine')
$items = $mach -split ';'
if ($items -notcontains $dir) {
  $new = ($items + $dir | Select-Object -Unique) -join ';'
  [Environment]::SetEnvironmentVariable('Path', $new, 'Machine')
}

Add-SystemPath