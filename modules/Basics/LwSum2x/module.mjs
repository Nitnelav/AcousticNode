export const name = 'LwSum2x';

export const caption = 'Lw Sum 2x';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw 1'},
    { id: "in2", type: 'spectrum', typeName: "Lw", description: 'Input Lw 2', required: false},
];

export const parameters = [
];

export const readonly_outputs = [
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var inputList = [];
    if (inputs["in1"]) {
        inputList.push(inputs["in1"])
    }
    if (inputs["in2"]) {
        inputList.push(inputs["in2"])
    }
    var outputLp = [-99.0, -99.0, -99.0, -99.0, -99.0, -99.0, -99.0, -99.0 ];

    for (let freq = 0; freq < 8; freq++) {
        inputList.forEach(input => {
            outputLp[freq] = 10 * Math.log10(Math.pow(10, outputLp[freq] / 10) + Math.pow(10, input[freq] / 10));
        });
    }
    return {
        "out1": output
    };
};

