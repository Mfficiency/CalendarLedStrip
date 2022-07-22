let showPrint = true;

async function print(text) {
    if (showPrint) {
        console.log(text);
    }
}

module.exports.Print = print;