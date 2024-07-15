import { httpRequest, isStr } from 'jsroot';

const _src = import.meta?.url;

let source_dir = 'go4sys/';

const version = '6.4.0';

if (isStr(_src)) {
   const pos = _src.indexOf('html5/core.mjs');
   if (pos >= 0)
      source_dir = _src.slice(0, pos);
}

console.log(`GO4 source_dir ${source_dir} version ${version}`);

/** @summary Execute method for selected painter object
    * @return {Promise} when done */
function executeMethod(painter, method, options) {
   let prefix = "";
   if (painter.getItemName())
      prefix = painter.getItemName() + "/"; // suppress / if item name is empty

   let fullcom = prefix + "exe.json?method=" + method + (options || "&"); // send any arguments otherwise ROOT refuse to process it

   return httpRequest(fullcom, 'text');
}



export { source_dir, version, executeMethod };