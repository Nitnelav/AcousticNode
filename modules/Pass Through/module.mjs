
const diff = 3;

export const name = 'LwPassThrough';

export const caption = 'Lw PassThrough';

export const inputs = [
    { type: 'Lw', description: 'Input Lw'},
];

export const parameters = [
];

export const outputs = [
    { type: 'Lw', description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputLw = inputs[0].map(function(el) {
        return el + diff;
    });
    return [ outputLw ];
};

