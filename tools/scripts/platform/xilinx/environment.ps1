$env_pth = $env:Path.Split(";")
$arg1 = $args[0]
$arg2 = $args[1]
$new_pth = "$arg1\Vivado\$arg2\bin","$arg1\Vivado\$arg2\msys64\usr\bin","$arg1\SDK\$arg2\bin","$arg1\SDK\$arg2\gnuwin\bin","$arg1\SDK\$arg2\gnu\microblaze\nt\bin","$arg1\SDK\$arg2\gnu\aarch32\nt\gcc-arm-none-eabi\bin","$arg1\SDK\$arg2\gnu\aarch64\nt\aarch64-none\bin"
$acc = $env:Path

foreach ($a in $new_pth){
  foreach ($b in $env_pth){
  if ($a -eq $b) {
    $exist = 1
    break
  }
  $exist = 0
  }
  if ($exist -eq 0) {
    $acc = $a + ";" + $acc
  }
}

setx PATH $acc
