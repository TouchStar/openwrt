#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/TS-NAV4
	NAME:=TouchStar NAV4
	PACKAGES:=kmod-usb-core kmod-usb2
endef
define Profile/TS-NAV4/Description
	Configuration of TouchStar NAV4.
endef

$(eval $(call Profile,TS-NAV4))
