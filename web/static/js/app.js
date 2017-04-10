const socket = io();

socket.on('build', data => {
    window.location.href = window.location.origin + data.url;
    $('.btn-build').removeAttr('disabled');
    $('.btn-build').html('Build');
});
socket.on('buildcount', data => {
    $('.lbl-buildcount').html(data.count + ' Builds');
});
socket.on('errormsg', data => {
    $('.box-error').removeClass('hidden');
    $('.box-error').html(data.message);
    $('.btn-build').html('Build');
    setTimeout(() => {
        $('.box-error').addClass('hidden');
        $('.btn-build').removeAttr('disabled');
    }, 10000);
});

$('.txt-cheat-name').change(function() {
    if ($(this).val().length < 1) {
        $('.name-error').removeClass('hidden');
        $('.name-box').addClass('has-error');
    } else {
        $('.name-error').addClass('hidden');
        $('.name-box').removeClass('has-error');
    }
});

$(document).ready(() => {
    socket.emit('buildcount');
    $('.ct-visible, .ct-invisible, .t-visible, .t-invisible').ColorPickerSliders({
        size: 'sm',
        placement: 'right',
        swatches: false,
        sliders: false,
        hsvpanel: true
    });

    $('.btn-build').click(() => {
        if ($('.txt-cheat-name').val().trim().length < 1 || $('.txt-cheat-name').val().trim()..length > 32 || $('.txt-cheat-name').val().trim().match(/^[\w\s-_]+$/i) === null) {
            $('.name-error').removeClass('hidden');
            $('.name-box').addClass('has-error');
            return;
        }
        $('.btn-build').attr('disabled', 'disabled');
        $('.btn-build').html('Build running&nbsp; <img width="24" src="/static/img/loader.svg" />');

        socket.emit('build', {
            name: $('.txt-cheat-name').val().trim(),
            ct: {
                visible: $('.ct-visible')[0].value.replace('rgb(', '').replace(')', '').split(', '),
                invisible: $('.ct-invisible')[0].value.replace('rgb(', '').replace(')', '').split(', ')
            },
            t: {
                visible: $('.t-visible')[0].value.replace('rgb(', '').replace(')', '').split(', '),
                invisible: $('.t-invisible')[0].value.replace('rgb(', '').replace(')', '').split(', ')
            }
        });
    });
});
