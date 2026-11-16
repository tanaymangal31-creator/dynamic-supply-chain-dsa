const { spawn, execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

console.log('================================================================');
console.log('  🚀 DYNAMIC SUPPLY CHAIN OPTIMIZATION & DISTRIBUTION SYSTEM');
console.log('  [Pure C++ Backend (Zero STL) + Live Web Dashboard]');
console.log('================================================================\n');

// 1. Kill any existing backend process if running
try {
    if (process.platform === 'win32') {
        execSync('taskkill /F /IM backend.exe 2>nul', { stdio: 'ignore' });
        execSync('taskkill /F /IM SpotifyLauncher.exe 2>nul', { stdio: 'ignore' });
    } else {
        execSync('pkill -f backend 2>/dev/null', { stdio: 'ignore' });
    }
} catch (e) {}

// 2. Check if re-compilation is needed
let needCompile = !fs.existsSync(path.join(__dirname, 'backend.exe'));
if (!needCompile && fs.existsSync(path.join(__dirname, 'backend.cpp'))) {
    const cppMtime = fs.statSync(path.join(__dirname, 'backend.cpp')).mtimeMs;
    const exeMtime = fs.statSync(path.join(__dirname, 'backend.exe')).mtimeMs;
    if (cppMtime > exeMtime) needCompile = true;
}

if (needCompile) {
    console.log('🔨 Compiling backend.cpp with MinGW g++ (Zero STL)...');
    try {
        const cmd = process.platform === 'win32'
            ? 'g++ -std=c++14 -O2 backend.cpp -o backend.exe -lws2_32'
            : 'g++ -std=c++14 -O2 backend.cpp -o backend';
        execSync(cmd, { cwd: __dirname, stdio: 'inherit' });
        console.log('✅ Compilation successful!\n');
    } catch (err) {
        console.error('❌ Compilation failed! Make sure g++ is installed and on your PATH.');
        process.exit(1);
    }
}

// 3. Start C++ backend process
const exe = process.platform === 'win32' 
    ? path.join(__dirname, 'backend.exe') 
    : path.join(__dirname, 'backend');

console.log('📡 Starting C++ backend on http://localhost:8080 ...');

const child = spawn(exe, [], {
    cwd: __dirname,
    stdio: 'inherit'
});

child.on('error', (err) => {
    console.error('Failed to start backend process:', err);
});

child.on('exit', (code) => {
    console.log(`Backend process exited with code ${code}`);
    process.exit(code || 0);
});

// 4. Open browser automatically after server boots
setTimeout(() => {
    console.log('\n🌐 Opening dashboard in your default browser: http://localhost:8080/');
    console.log('👉 Press Ctrl+C in this terminal anytime to stop the server.\n');
    try {
        const openCmd = process.platform === 'win32' ? 'start http://localhost:8080/' :
                        process.platform === 'darwin' ? 'open http://localhost:8080/' :
                        'xdg-open http://localhost:8080/';
        execSync(openCmd, { stdio: 'ignore' });
    } catch (e) {}
}, 1200);

// 5. Handle Ctrl+C clean shutdown
function shutdown() {
    console.log('\n🛑 Shutting down backend server...');
    try {
        if (process.platform === 'win32') {
            execSync('taskkill /F /IM backend.exe 2>nul', { stdio: 'ignore' });
        } else {
            child.kill();
        }
    } catch (e) {}
    process.exit(0);
}

process.on('SIGINT', shutdown);
process.on('SIGTERM', shutdown);
