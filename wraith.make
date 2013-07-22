; Core makefile for AAI drupal sites.
; Run drush make ash_core.make
api = 2
core = 7.x

; Sonar
;
; Compass compiler for Drupal
; @see https://github.com/JaceRider/Sonar
projects[sonar][subdir]                       = "contrib"
projects[sonar][type]                         = "module"
projects[sonar][download][type]               = "git"
projects[sonar][download][url]                = "https://github.com/JaceRider/Sonar.git"
projects[sonar][download][branch]       			= "1.0"

; Fawesome
;
; Add Font Awesome to Drupal.
; @see https://github.com/JaceRider/Fawesome
projects[fawesome][subdir]                       = "contrib"
projects[fawesome][type]                         = "module"
projects[fawesome][download][type]               = "git"
projects[fawesome][download][url]                = "https://github.com/JaceRider/Fawesome.git"
projects[fawesome][download][branch]       			= "1.0"
