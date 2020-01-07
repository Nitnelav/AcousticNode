
export const FREQ_63Hz = 0;
export const FREQ_125Hz = 1;
export const FREQ_250Hz = 2;
export const FREQ_500Hz = 3;
export const FREQ_1kHz = 4;
export const FREQ_2kHz = 5;
export const FREQ_4kHz = 6;
export const FREQ_8kHz = 7;

export class Spectrum {

    constructor(array) {
        this.data_ = [];
        if (array && Array.isArray(array) && array.length >= FREQ_8kHz) {
            for (let freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
                this.data_[freq] = array[freq];
            }
        }
    }

    getValue(freq) {
        return this.data_[freq];
    }

    setValue(freq, value) {
        this.data_[freq] = value;
    }

    add(other) {
        if (other instanceof Spectrum) {
            return new Spectrum(this.data_.map((val, freq) => {
                return val + other.getValue(freq);
            }));
        }
        if (other instanceof Number) {
            return new Spectrum(this.data_.map(val => {
                return val + other;
            }));
        }
    }

    sub(other) {
        if (other instanceof Spectrum) {
            return new Spectrum(this.data_.map((val, freq) => {
                return val - other.getValue(freq);
            }));
        }
        if (other instanceof Number) {
            return new Spectrum(this.data_.map(val => {
                return val - other;
            }));
        }
    }

    map(callback) {
        return this.data_.map(callback);
    }
}