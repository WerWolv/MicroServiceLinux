# Raspberry Pi boot firmware

`start4.elf` and `fixup4.dat` are vendored from the Raspberry Pi firmware
repository at commit `dcca4969e53d2a25e688f0f228a09486786d54c0`:

https://github.com/raspberrypi/firmware/tree/dcca4969e53d2a25e688f0f228a09486786d54c0/boot

They are distributed under `LICENCE.broadcom` and are used only to start the
upstream U-Boot image on Raspberry Pi hardware.

SHA-256 checksums:

```text
560ead37044d849325755d3684ac1f74654b3ce6450604a42df6dd832cda9ede  start4.elf
d181d5055daebac22513d6904fa9ce21d1e99b852cc391af095ade6f0b45c400  fixup4.dat
5657db912dd2007ddc2c0c71671a7f45e41bc63bb82680e6a107f2064da5e088  LICENCE.broadcom
```
