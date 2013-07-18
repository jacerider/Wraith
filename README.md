# Wraith


## Base theme for Drupal that offers the following features:

+ [Twitter Bootstrap](http://twitter.github.io/bootstrap/) frontend
+ [Sass](http://sass-lang.com/) and [Compass](http://compass-style.org/) for semantically-awesome stylesheets
+ [Coffeescript](http://coffeescript.org/) support/compilation
+ Ability to choose which stylesheets or javascripts are loaded either on a per-module or per-file basis
+ Auto compresses and minifies javascript and stylesheet files

## Usage

Wraith is meant to be a stable base to build sub-themes on top of, so I highly suggest you generate a sub-theme 
of Wraith with the following `drush` command and follow the prompts where necessary.

```shell
drush -l mysite.com wraith "casper"
```


This will generate the `casper` theme in your `sites/all/themes` directory and enable the `casper` theme in your
database. You will also be asked a yes or no question if you would like to set this new theme as your default theme.
