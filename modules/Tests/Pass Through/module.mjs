
const diff = 3;

export const name = 'LwPassThrough';

export const caption = 'Lw PassThrough';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
    { id: "in2", type: 'int', typeName: "Flow", description: 'Flow rate (m3/s)', required: false },
];

export const parameters = [
    { id: "param1", type: 'bool', typeName: "Bool", description: 'Add +3 dB ?'},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { id: "out2", type: 'int', typeName: "Flow", description: 'Flow rate (m3/s)' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    var applyDiff = parameters["param1"] === true ? diff : 0;
    outputs["out1"] = inputs["in1"].map(function(el) {
        return el + applyDiff;
    });
    if (inputs["in2"]) {
        outputs["out2"] = inputs["in2"];
    }
    return outputs;
};

