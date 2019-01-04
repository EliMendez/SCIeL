$(function () {
  $.ajaxSetup({
      headers: { "X-CSRFToken": getCookie("csrftoken") }
  });
});