const {maxNodes} = require('./Values');
const {read, update, requestKeyList} = require('./DataStore');
const {getAllNodesInfo} = require('./NodeStore.js');


const CommandProcessors = {
    setMaxNodes: num => maxNodes.set(num),
    requestAllNodes: (data, connection) =>
        connection.send(JSON.stringify({cmd: 'updateNodes', data: getAllNodesInfo()})),
    read,
    update,
    // delete
    // aggregate

    aggregate: (data, ws) => {
        data.forEach(cmd => {
            CommandProcessors[cmd.cmd](cmd.data, ws);
        });
    },

    requestKeyList
};


module.exports = CommandProcessors;



