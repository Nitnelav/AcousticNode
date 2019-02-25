
const diff = 3;

export const name = 'LwPassThrough';

export const caption = 'Lw PassThrough';

export const inputs = [
    { type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { type: 'bool', typeName: "Bool", description: 'Add +3 dB ?'},
];

export const outputs = [
    { type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var applyDiff = parameters[0] === true ? diff : 0;
    var outputLw = inputs[0].map(function(el) {
        return el + applyDiff;
    });
    return [ outputLw ];
};

