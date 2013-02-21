(function ($) {

/**
 * Add button element to views ajax processing
 */
if(Drupal.views && Drupal.views.ajaxView){
  Drupal.views.ajaxView.prototype.attachExposedFormAjax = function() {
    var button = $('input[type=submit], input[type=image], button', this.$exposed_form);
    button = button[0];

    this.exposedFormAjax = new Drupal.ajax($(button).attr('id'), button, this.element_settings);
  };
}

})(jQuery);
