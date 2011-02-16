# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS   = plugin_coreplugin

# Blank Template Plugin, not compiled by default
#SUBDIRS += plugin_donothing
#plugin_donothing.subdir = donothing
#plugin_donothing.depends = plugin_coreplugin


# Core plugin
plugin_coreplugin.subdir = coreplugin

# Empty UAVGadget - Default for new splits
plugin_emptygadget.subdir = emptygadget
plugin_emptygadget.depends = plugin_coreplugin
SUBDIRS += plugin_emptygadget

# Welcome plugin
plugin_welcome.subdir = welcome
plugin_welcome.depends = plugin_coreplugin
SUBDIRS += plugin_welcome

# RawHID connection plugin
SUBDIRS += plugin_rawhid
plugin_rawhid.subdir = rawhid
plugin_rawhid.depends = plugin_coreplugin

# Serial port connection plugin
SUBDIRS += plugin_serial
plugin_serial.subdir = serialconnection
plugin_serial.depends = plugin_coreplugin

# UAVObjects plugin
SUBDIRS += plugin_uavobjects
plugin_uavobjects.subdir = uavobjects
plugin_uavobjects.depends = plugin_coreplugin

# UAVTalk plugin
SUBDIRS += plugin_uavtalk
plugin_uavtalk.subdir = uavtalk
plugin_uavtalk.depends = plugin_uavobjects
plugin_uavtalk.depends += plugin_coreplugin

# OPMap UAVGadget
plugin_opmap.subdir = opmap
plugin_opmap.depends = plugin_coreplugin
plugin_opmap.depends += plugin_uavobjects
SUBDIRS += plugin_opmap

# Scope UAVGadget
plugin_scope.subdir = scope
plugin_scope.depends = plugin_coreplugin
plugin_scope.depends += plugin_uavobjects
SUBDIRS += plugin_scope

# UAVObject Browser gadget
plugin_uavobjectbrowser.subdir = uavobjectbrowser
plugin_uavobjectbrowser.depends = plugin_coreplugin
plugin_uavobjectbrowser.depends += plugin_uavobjects
SUBDIRS += plugin_uavobjectbrowser

# ModelView UAVGadget
plugin_modelview.subdir = modelview
plugin_modelview.depends = plugin_coreplugin
plugin_modelview.depends += plugin_uavobjects
SUBDIRS += plugin_modelview

#Uploader gadget
plugin_uploader.subdir = uploader
plugin_uploader.depends = plugin_coreplugin
plugin_uploader.depends += plugin_uavobjects
plugin_uploader.depends += plugin_rawhid
SUBDIRS += plugin_uploader

#Dial gadget
plugin_dial.subdir = dial
plugin_dial.depends = plugin_coreplugin
plugin_dial.depends += plugin_uavobjects
SUBDIRS += plugin_dial

#Linear Dial gadget
plugin_lineardial.subdir = lineardial
plugin_lineardial.depends = plugin_coreplugin
plugin_lineardial.depends += plugin_uavobjects
SUBDIRS += plugin_lineardial

#System Health gadget
plugin_systemhealth.subdir = systemhealth
plugin_systemhealth.depends = plugin_coreplugin
plugin_systemhealth.depends += plugin_uavobjects
SUBDIRS += plugin_systemhealth

#Notify gadget
plugin_notify.subdir = notify
plugin_notify.depends = plugin_coreplugin
plugin_notify.depends += plugin_uavobjects
SUBDIRS += plugin_notify

#Config gadget
plugin_config.subdir = config
plugin_config.depends = plugin_coreplugin
plugin_config.depends += plugin_uavobjects
SUBDIRS += plugin_config

#GPS Display gadget
plugin_gpsdisplay.subdir = gpsdisplay
plugin_gpsdisplay.depends = plugin_coreplugin
plugin_gpsdisplay.depends += plugin_uavtalk
SUBDIRS += plugin_gpsdisplay

# Primary Flight Display (PFD) gadget
plugin_pfd.subdir = pfd
plugin_pfd.depends = plugin_coreplugin
plugin_pfd.depends += plugin_uavobjects
SUBDIRS += plugin_pfd

#IP connection plugin
plugin_ipconnection.subdir = ipconnection
plugin_ipconnection.depends = plugin_coreplugin
SUBDIRS += plugin_ipconnection

#HITLNEW Simulation gadget
plugin_hitlnew.subdir = hitlnew
plugin_hitlnew.depends = plugin_coreplugin
plugin_hitlnew.depends += plugin_uavobjects
plugin_hitlnew.depends += plugin_uavtalk
SUBDIRS += plugin_hitlnew

# Export and Import GCS Configuration
plugin_importexport.subdir = importexport
plugin_importexport.depends = plugin_coreplugin
SUBDIRS += plugin_importexport

# Telemetry data logging plugin
plugin_logging.subdir = logging
plugin_logging.depends = plugin_coreplugin
plugin_logging.depends += plugin_uavobjects
plugin_logging.depends += plugin_uavtalk
SUBDIRS += plugin_logging

#GCS Control of UAV gadget
plugin_gcscontrol.subdir = gcscontrol
plugin_gcscontrol.depends = plugin_coreplugin
plugin_gcscontrol.depends += plugin_uavobjects
plugin_gcscontrol.depends += plugin_uavtalk
SUBDIRS += plugin_gcscontrol

# Antenna tracker
#plugin_antennatrack.subdir = antennatrack
#plugin_antennatrack.depends = plugin_coreplugin
#plugin_antennatrack.depends += plugin_uavtalk
#SUBDIRS += plugin_antennatrack

#PipXtreme gadget
plugin_pipxtreme.subdir = pipxtreme
plugin_pipxtreme.depends = plugin_coreplugin
plugin_pipxtreme.depends += plugin_uavobjects
plugin_pipxtreme.depends += plugin_rawhid
SUBDIRS += plugin_pipxtreme

#Scope2 gadget
#plugin_scope2.subdir = scope2
#plugin_scope2.depends = plugin_coreplugin
#plugin_scope2.depends += plugin_uavobjects
#SUBDIRS += plugin_scope2

# Magic Waypoint gadget
plugin_magicwaypoint.subdir = magicwaypoint
plugin_magicwaypoint.depends = plugin_coreplugin
SUBDIRS += plugin_magicwaypoint

# UAV Settings Import/Export plugin
plugin_uavsettingsimportexport.subdir = uavsettingsimportexport
plugin_uavsettingsimportexport.depends = plugin_coreplugin
plugin_uavsettingsimportexport.depends += plugin_uavobjects
SUBDIRS += plugin_uavsettingsimportexport
