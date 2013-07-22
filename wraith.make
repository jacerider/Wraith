; Core makefile for AAI drupal sites.
; Run drush make ash_core.make
api = 2
core = 7.x

; Icon API
;
; integration for icon bundles and icon providers throughout Drupal.
; @see https://drupal.org/project/icon
projects[icon][subdir]                       = "contrib"
projects[icon][version]                      = "1.0-beta2"
