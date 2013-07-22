; Core makefile for AAI drupal sites.
; Run drush make ash_core.make
api = 2
core = 7.x

; Icon API
;
; integration for icon bundles and icon providers throughout Drupal.
; @see https://drupal.org/project/icon
projects[icon][subdir]                          = "contrib"
projects[icon][version]                         = "1.0-beta2"

; Fontello
;
; This module integrates the amazing Fontello service via the Icon API and
; allows you combine icon webfonts for your own project.
; @see https://drupal.org/project/fontello
projects[fontello][subdir]                       = "contrib"
projects[fontello][version]                      = "1.0"
