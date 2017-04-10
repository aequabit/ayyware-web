module.exports = {
    IsValidRgbColor: function(color) {
        var valid = true;
        color.forEach((entry) => {
            if (entry > 255 || entry < 0)
                valid = false;
        });
        return valid;
    }
};
