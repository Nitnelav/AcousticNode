export const name = 'LwSumToLp';

export const caption = 'Lw Sum to Lp';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input 1', required: false},
    { id: "in2", type: 'spectrum', typeName: "Lw", description: 'Input 2', required: false},
    { id: "in3", type: 'spectrum', typeName: "Lw", description: 'Input 3', required: false},
    { id: "in4", type: 'spectrum', typeName: "Lw", description: 'Input 4', required: false},
    { id: "in5", type: 'spectrum', typeName: "Lw", description: 'Input 5', required: false},
];

export const parameters = [
    { id: "param1", type: 'choice', typeName: "Choice", description: 'What to do', choices: ["sum", "sum+3"] },
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lp", description: 'Output Lp' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    var inputList = [ ];
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
    if (inputs["in5"]) {
        inputList.push(inputs["in5"])
    }
    var outputLp = [-99.0, -99.0, -99.0, -99.0, -99.0, -99.0, -99.0, -99.0 ];
    for (let freq = 0; freq < 8; freq++) {
        inputList.forEach(input => {
            outputLp[freq] = 10*Math.log10(Math.pow(10,outputLp[freq]/10) + Math.pow(10,input[freq]/10));
        });
    }
    if (parameters["param1"] === "sum+3") {
        for (let freq = 0; freq < 8; freq++) {
            outputLp[freq] = outputLp[freq] + 3
        }
    }
    outputs["out1"] = outputLp;
    return outputs;
};

