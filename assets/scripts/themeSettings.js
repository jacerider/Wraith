/*
 * ##### Sasson - advanced drupal theming. #####
 *
 * This script will add a draggable overlay image you can lay over your HTML
 * for easy visual comparison.
 * Image source and opacity are set via theme settings form
 *
 */

(function($) {
  
  Drupal.behaviors.themeSettings = {
    attach: function(context, settings) {

      // Select all/none
      $('a.select-all, a.select-none').once('select-all').click(function(e) {
        e.preventDefault();
        var check = $(this).is('.select-all') ? true : false;
        $(this).parents('fieldset:eq(0)').find(':checkbox').attr('checked', check);
      });

      // Toggle extension
      $('input.enable-extension').once('enable-extension').each(function(e) {
        var extension = $(this).parent('.form-item').siblings();

        if($(this).is(':checked')) {
          extension.fadeTo('slow', 1);
        } else {
          extension.fadeTo('slow', 0.3);
        }

        $(this).click(function() {
          if($(this).is(':checked')) {
            extension.fadeTo('slow', 1);
          } else {
            extension.fadeTo('slow', 0.3);
          }
        });
      });

      // Toggle extension
      $('input.disable-extension').once('disable-extension').each(function(e) {
        var extension = $(this).parent('.form-item').siblings();

        if($(this).is(':checked')) {
          extension.fadeTo('slow', 0.3);
        } else {
          extension.fadeTo('slow', 1);
        }

        $(this).click(function() {
          if($(this).is(':checked')) {
            extension.fadeTo('slow', 0.3);
          } else {
            extension.fadeTo('slow', 1);
          }
        });
      });

      $('input[data-disable]').once('disable-extension').each(function(e) {
        var extension = $(this).attr('data-disable');
        extension = $(extension);
        if(!extension.length) return;

        if($(this).is(':checked')) {
          extension.fadeTo('slow', 0.3);
        } else {
          extension.fadeTo('slow', 1);
        }

        $(this).click(function() {
          if($(this).is(':checked')) {
            extension.fadeTo('slow', 0.3);
          } else {
            extension.fadeTo('slow', 1);
          }
        });
      });

    }
  };

})(jQuery);
