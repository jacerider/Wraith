/*
 * ##### wraith - advanced drupal theming. #####
 *
 * This script will watch files for changes and
 * automatically update the browser when a file is modified.
 *
 * Usage:
 *
 *     Drupal.wraith.watch('/path/to/your/stylesheet.css', true);
 *
 */

(function($) {
  
  Drupal.wraith = Drupal.wraith || {};

  Drupal.wraith.watch = function(url, instant, actual) {

    var dateModified, lastDateModified, init;

    var updateStyle = function(filename) {
      var headElm = $('head > link[href*="' + filename + '.css"]');
      if (headElm.length > 0) {
        // If it's in a <link> tag
        headElm.attr('href', headElm.attr('href').replace(filename + '.css?', filename + '.css?s'));
      } else {
        // If it's in an @import rule
        headElm = $("head > *:contains('" + filename + ".css')");
        if(headElm.length > 0){
          headElm.html(headElm.html().replace(filename + '.css?', filename + '.css?s'));
        }else{

        }
      }
    };
    
    // Check every second if the timestamp was modified
    var check = function(dateModified) {
      if (init === true && lastDateModified !== dateModified) {
        var filename = url.split('/');
        filename = filename[filename.length - 1].split('.');
        var fileExt = filename[1];
        filename = filename[0];
        if (instant && fileExt === 'css') {
          // css file - update head
          updateStyle(filename);
        } else if (instant && (fileExt === 'scss' || fileExt === 'sass')) {
          // SASS/SCSS file - trigger sass compilation with an ajax call and update head
          $.ajax({
            url: document.URL,
            success: function() {
              filename = actual.length ? actual : filename;
              updateStyle(filename);
            }
          });
        } else {
          // Reload the page
          document.location.reload(true);
        }
      }
      init = true;
      lastDateModified = dateModified;
    };

    var watch = function(url) {
      $.ajax({
        url: url + '?' + Math.random(),
        type:"HEAD",
        error: function() {
          log(Drupal.t('There was an error watching @url', {'@url': url}));
          clearInterval(watchInterval);
        },
        success:function(res,code,xhr) {
          check(xhr.getResponseHeader("Last-Modified"));
        }
      });
    };
    
    var watchInterval = 0;
    watchInterval = window.setInterval(function() {
      watch(url);
    }, 1000);

  };
  
})(jQuery);
