export const name = 'LwSum4x';

export const caption = 'Lw Sum 4x';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw 1'},
    { id: "in2", type: 'spectrum', typeName: "Lw", description: 'Input Lw 2', required: false},
    { id: "in3", type: 'spectrum', typeName: "Lw", description: 'Input Lw 3', required: false},
    { id: "in4", type: 'spectrum', typeName: "Lw", description: 'Input Lw 4', required: false},
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
    if (inputs["in3"]) {
        inputList.push(inputs["in3"])
    }
    if (inputs["in4"]) {
        inputList.push(inputs["in4"])
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

