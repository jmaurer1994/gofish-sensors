[env]
build_flags =
    -DDEBUG
    -DINITIAL_STARTUP_DELAY=5000
    -DWLAN_IPV4_ADDRESS=192,168,1,120
    -DWLAN_IPV4_GATEWAY=192,168,1,1
    -DWLAN_IPV4_SUBNET_MASK=255,255,255,0

    '-DWLAN_SSID=""'
    '-DWLAN_WPA2KEY=""'