$(function () {
    function appendZero(val) {
        if (parseInt(val) < 10) {
            return "0"+val;
        }
        else{
            return val;
        }
    }
    function populateReadings() {
        $.ajax({
            url: '/ping-of-things/readings/',
            type: 'GET',
            success: function (response) {
                if (response.environment_temperature != 0) {
                    $('#env_temperature').find('p').html(appendZero(response.environment_temperature));
                    $('#env_temperature').find('span.superscript').show();
                }
                else {
                    $('#env_temperature').find('p').html("N/A");
                    $('#env_temperature').find('span.superscript').hide();

                }
                if (response.body_temperature != 0) {
                    if (response.btThreshold == 1) {
                        $('#body_temperature').find('p').css('color', 'red');
                    }
                    else {
                        $('#body_temperature').find('p').css('color', 'black');
                    }
                    $('#body_temperature').find('p').html(appendZero(response.body_temperature));
                    $('#body_temperature').find('span.superscript').show();
                }
                else {
                    $('#body_temperature').find('p').html("N/A");
                    $('#body_temperature').find('span.superscript').hide();

                }
                if (response.humidity != 0) {
                    $('#humidity').find('p').html(appendZero(response.humidity));
                    $('#humidity').find('span.superscriptPercentage').show();
                }
                else {
                    $('#humidity').find('p').html("N/A");
                    $('#humidity').find('span.superscriptPercentage').hide();
                }
                if (response.heart_rate != 0) {
                    if (response.hrThreshold == 1)
                    {
                        $('#heart_rate').find('p').css('color', 'red');    
                    }
                    else {
                        $('#heart_rate').find('p').css('color', 'black');  
                    }
                    $('#heart_rate').find('span.superscriptBPM').show();                    
                    $('#heart_rate').find('p').html(appendZero(response.heart_rate));
                }
                else {
                    $('#heart_rate').find('p').html("N/A");
                    $('#heart_rate').find('span.superscriptBPM').hide();
                }
                if (response.blood_oxygen != 0) {
                    if (response.boThreshold == 1){
                        $('#blood_oxygen').find('p').css('color', 'red'); 
                    }
                    else {
                        $('#blood_oxygen').find('p').css('color', 'black'); 
                    }   
                    $('#blood_oxygen').find('p').html(appendZero(response.blood_oxygen));
                    $('#blood_oxygen').find('span.superscriptPercentage').show();
                }
                else {
                    $('#blood_oxygen').find('p').html("N/A");
                    $('#blood_oxygen').find('span.superscriptPercentage').hide();
                }
            },
            error: function (x, e) {

            }
        });
    }
    setInterval(populateReadings, 5000);
    populateReadings();
});