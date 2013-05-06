(function($) {
  $(document).ready(function() {
    $('input[type="checkbox"].toggle-details').bind('change', function() {
      var $this = $(this),
          $targ = $($this.attr('data-to-toggle')),
          targ_html = $targ.html();
      
      if (typeof targ_html == 'string') {
        if (($this.is(':checked') && !$targ.is(':visible')) || (!$this.is(':checked') && $targ.is(':visible'))) {
          $targ.slideToggle(150);
        }
      }
    });
  });
})(jQuery);