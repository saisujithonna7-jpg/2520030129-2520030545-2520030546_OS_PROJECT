/**
 * app.js
 * Main Client Controller for Interactive Operating System System Calls Simulator.
 * Manages view routing, module data, C code editing, GCC compilation execution,
 * ANSI terminal rendering, strace auditing, and viva examinations.
 */

// Global State
const State = {
  currentView: 'home',
  modules: [],
  explorerSyscalls: [],
  currentModule: null,
  currentDemo: null,
  currentCode: '',
  useStrace: false,
  activeTerminalTab: 'stdout',
  lastExecution: null,
  systemInfo: null,
  isLoading: false
};

// ANSI Color Code to HTML Span Converter
function ansiToHtml(text) {
  if (!text) return '';
  return text
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/\033\[1;31m/g, '<span style="color: #fb7185; font-weight: bold;">')
    .replace(/\033\[1;32m/g, '<span style="color: #34d399; font-weight: bold;">')
    .replace(/\033\[1;33m/g, '<span style="color: #fbbf24; font-weight: bold;">')
    .replace(/\033\[1;34m/g, '<span style="color: #60a5fa; font-weight: bold;">')
    .replace(/\033\[1;35m/g, '<span style="color: #c084fc; font-weight: bold;">')
    .replace(/\033\[1;36m/g, '<span style="color: #38bdf8; font-weight: bold;">')
    .replace(/\033\[1;37m/g, '<span style="color: #f8fafc; font-weight: bold;">')
    .replace(/\033\[0m/g, '</span>');
}

// App Initializer
document.addEventListener('DOMContentLoaded', async () => {
  setupNavigation();
  setupTerminalTabs();
  setupExecutionActions();
  await fetchSystemInfo();
  await fetchModules();
  await fetchExplorerData();
});

// View Navigation
function setupNavigation() {
  const navLinks = document.querySelectorAll('.nav-link[data-view]');
  navLinks.forEach(link => {
    link.addEventListener('click', (e) => {
      e.preventDefault();
      const targetView = link.getAttribute('data-view');
      switchView(targetView);
    });
  });

  const brand = document.querySelector('.brand');
  if (brand) {
    brand.addEventListener('click', () => switchView('home'));
  }
}

function switchView(viewName) {
  State.currentView = viewName;
  document.querySelectorAll('.view-section').forEach(sec => sec.classList.remove('active'));
  document.querySelectorAll('.nav-link').forEach(nl => nl.classList.remove('active'));

  const activeSec = document.getElementById(`view-${viewName}`);
  if (activeSec) activeSec.classList.add('active');

  const activeLink = document.querySelector(`.nav-link[data-view="${viewName}"]`);
  if (activeLink) activeLink.classList.add('active');

  window.scrollTo({ top: 0, behavior: 'smooth' });
}

// Fetch Host & Linux Kernel Info
async function fetchSystemInfo() {
  try {
    const res = await fetch('/api/system-info');
    if (res.ok) {
      State.systemInfo = await res.json();
      const statusKernel = document.getElementById('kernelStatusText');
      if (statusKernel) {
        statusKernel.textContent = `${State.systemInfo.os_name} (${State.systemInfo.gcc_version || 'GCC 15'})`;
      }
      const uptimeElem = document.getElementById('heroStatUptime');
      if (uptimeElem && State.systemInfo.kernel) {
        uptimeElem.textContent = State.systemInfo.kernel.split(' ')[0] || 'Linux 6.x';
      }
    }
  } catch (err) {
    console.warn('System info offline:', err);
  }
}

// Fetch Modules Catalog
async function fetchModules() {
  try {
    const res = await fetch('/api/modules');
    if (res.ok) {
      State.modules = await res.json();
      renderHomeModules();
      renderVivaModuleTabs();

      // Default select first demo
      if (State.modules.length > 0 && State.modules[0].demos.length > 0) {
        selectDemo(State.modules[0].demos[0].id, false);
      }
    }
  } catch (err) {
    console.error('Failed to load modules:', err);
  }
}

// Fetch System Call Explorer Directory
async function fetchExplorerData() {
  try {
    const res = await fetch('/api/explorer');
    if (res.ok) {
      State.explorerSyscalls = await res.json();
      renderExplorerTable(State.explorerSyscalls);
      setupExplorerSearch();
    }
  } catch (err) {
    console.error('Failed to load explorer data:', err);
  }
}

// Render Course Outcome Cards on Home View
function renderHomeModules() {
  const grid = document.getElementById('homeCoGrid');
  if (!grid) return;

  grid.innerHTML = '';
  State.modules.forEach(mod => {
    const card = document.createElement('div');
    card.className = 'co-card';
    card.innerHTML = `
      <div class="co-card-top">
        <span class="co-badge">${mod.code}</span>
        <span class="co-icon">⚡</span>
      </div>
      <h3 class="co-title">${mod.title}</h3>
      <p class="co-desc">${mod.summary}</p>
      <div class="co-meta">
        ${mod.demos.slice(0, 3).map(d => `<span class="co-tag">${d.title}</span>`).join('')}
        ${mod.demos.length > 3 ? `<span class="co-tag">+${mod.demos.length - 3} more</span>` : ''}
      </div>
      <div class="co-action">
        <span>Explore ${mod.demos.length} Demonstrations</span>
        <span>→</span>
      </div>
    `;

    card.addEventListener('click', () => {
      openModuleStudio(mod.id);
    });

    grid.appendChild(card);
  });
}

// Open Specific Course Outcome in Studio View
function openModuleStudio(moduleId, targetDemoId = null) {
  const mod = State.modules.find(m => m.id === moduleId);
  if (!mod) return;

  State.currentModule = mod;
  renderSidebarDemos(mod);

  const demoToSelect = targetDemoId || mod.demos[0].id;
  selectDemo(demoToSelect, true);
  switchView('studio');
}

// Render Sidebar List of Demonstrations
function renderSidebarDemos(mod) {
  const listContainer = document.getElementById('studioDemoNavList');
  const sidebarHeader = document.getElementById('studioSidebarModuleTitle');
  if (sidebarHeader) sidebarHeader.textContent = `${mod.code}: ${mod.title}`;
  if (!listContainer) return;

  listContainer.innerHTML = '';
  mod.demos.forEach(demo => {
    const item = document.createElement('div');
    item.className = `demo-nav-item ${State.currentDemo && State.currentDemo.id === demo.id ? 'active' : ''}`;
    item.setAttribute('data-demo-id', demo.id);
    item.innerHTML = `
      <div class="demo-nav-header">
        <div class="demo-nav-name">${demo.title}</div>
        <span class="demo-nav-diff diff-${demo.difficulty}">${demo.difficulty}</span>
      </div>
      <div class="demo-nav-syscalls">${demo.syscalls.map(s => `${s}()`).join(', ')}</div>
    `;

    item.addEventListener('click', () => {
      selectDemo(demo.id, true);
    });

    listContainer.appendChild(item);
  });
}

// Select a Demonstration and Load Code & Explanations
async function selectDemo(demoId, switchStudio = true) {
  // Find module & demo
  let foundMod = null;
  let foundDemo = null;

  for (const mod of State.modules) {
    const d = mod.demos.find(x => x.id === demoId);
    if (d) {
      foundMod = mod;
      foundDemo = d;
      break;
    }
  }

  if (!foundDemo) return;

  State.currentModule = foundMod;
  State.currentDemo = foundDemo;

  // Update active sidebar item
  document.querySelectorAll('.demo-nav-item').forEach(el => {
    if (el.getAttribute('data-demo-id') === demoId) {
      el.classList.add('active');
    } else {
      el.classList.remove('active');
    }
  });

  // Fetch C Source Code
  try {
    const res = await fetch(`/api/demo/${demoId}`);
    if (res.ok) {
      const data = await res.json();
      State.currentCode = data.code;
      renderDemoWorkspace(data.demo, data.module, data.code);
    }
  } catch (err) {
    console.error('Error fetching demo code:', err);
  }

  if (switchStudio && State.currentView !== 'studio') {
    switchView('studio');
  }
}

// Render Studio Header, Code Viewer, Explanation, and Flow Diagram
function renderDemoWorkspace(demo, moduleInfo, code) {
  // 1. Header info
  const titleElem = document.getElementById('studioDemoTitle');
  const descElem = document.getElementById('studioDemoDesc');
  const modTag = document.getElementById('studioModuleTag');
  const diffTag = document.getElementById('studioDiffTag');
  const syscallTags = document.getElementById('studioSyscallTags');

  if (titleElem) titleElem.textContent = demo.title;
  if (descElem) descElem.textContent = demo.summary;
  if (modTag) modTag.textContent = `${moduleInfo.code}: ${moduleInfo.title}`;
  if (diffTag) {
    diffTag.textContent = demo.difficulty;
    diffTag.className = `demo-nav-diff diff-${demo.difficulty}`;
  }
  if (syscallTags) {
    syscallTags.innerHTML = demo.syscalls.map(s => `<span class="syscall-pill">${s}()</span>`).join(' ');
  }

  // 2. Code Textarea
  const textarea = document.getElementById('cCodeTextarea');
  if (textarea) {
    textarea.value = code;
  }

  // 3. Explanation text
  const explanationDiv = document.getElementById('demoExplanationContent');
  if (explanationDiv) {
    explanationDiv.innerHTML = `<p class="explanation-text">${demo.explanation}</p>`;
  }

  // 4. Execution Flow Diagram
  const diagramContainer = document.getElementById('demoFlowDiagramContainer');
  if (diagramContainer) {
    DiagramVisualizer.renderFlowChain(demo.kernel_flow, diagramContainer);
  }

  // 5. Contextual Memory / Process Visualizers
  const extraVisualizer = document.getElementById('extraVisualizerContainer');
  if (extraVisualizer) {
    extraVisualizer.innerHTML = '';
    if (demo.id === 'paging_translation' || demo.id === 'mmap_demo' || demo.id === 'demand_paging') {
      DiagramVisualizer.renderMemoryCalculator('extraVisualizerContainer');
    } else if (demo.id === 'fork_demo' || demo.id === 'process_tree' || demo.id === 'wait_exit') {
      DiagramVisualizer.renderProcessTreeSVG(extraVisualizer);
    }
  }

  // 6. Viva Questions for this demo
  renderDemoViva(demo.viva_questions);

  // 7. Reset Terminal output to ready state
  resetTerminal(demo);
}

// Render Viva Questions Accordion
function renderDemoViva(vivaList) {
  const container = document.getElementById('demoVivaContainer');
  if (!container) return;

  if (!vivaList || vivaList.length === 0) {
    container.innerHTML = '<div style="color: var(--text-dim);">No specific viva questions for this unit.</div>';
    return;
  }

  let html = '<div class="viva-list">';
  vivaList.forEach((item, idx) => {
    html += `
      <div class="viva-card">
        <div class="viva-question" onclick="toggleVivaCard(this)">
          <span>Q${idx + 1}: ${item.q}</span>
          <span class="viva-chevron">▼</span>
        </div>
        <div class="viva-answer">
          <p>${item.a}</p>
          ${item.tip ? `<div class="viva-tip"><strong>Viva Tip:</strong> ${item.tip}</div>` : ''}
        </div>
      </div>
    `;
  });
  html += '</div>';
  container.innerHTML = html;
}

window.toggleVivaCard = function(questionElem) {
  const card = questionElem.closest('.viva-card');
  if (card) card.classList.toggle('open');
};

// Reset Terminal Window
function resetTerminal(demo) {
  const terminalOut = document.getElementById('terminalOutputArea');
  const statusBadge = document.getElementById('terminalStatusBadge');
  const timerBadge = document.getElementById('terminalTimerBadge');

  if (terminalOut) {
    terminalOut.innerHTML = `Ready to compile and run <strong>${demo.title}</strong> on Ubuntu Linux.\nClick 'Compile & Run on Ubuntu' above to execute with GCC.`;
  }
  if (statusBadge) {
    statusBadge.textContent = 'READY';
    statusBadge.className = 'status-badge';
  }
  if (timerBadge) {
    timerBadge.textContent = '0.0 ms';
  }
}

// Setup Terminal Tab Switching (STDOUT, STDERR, GCC LOG, STRACE)
function setupTerminalTabs() {
  const tabs = document.querySelectorAll('.panel-tab[data-term-tab]');
  tabs.forEach(tab => {
    tab.addEventListener('click', () => {
      tabs.forEach(t => t.classList.remove('active'));
      tab.classList.add('active');
      State.activeTerminalTab = tab.getAttribute('data-term-tab');
      displayTerminalContent();
    });
  });

  const clearBtn = document.getElementById('btnClearTerminal');
  if (clearBtn) {
    clearBtn.addEventListener('click', () => {
      const terminalOut = document.getElementById('terminalOutputArea');
      if (terminalOut) terminalOut.innerHTML = 'Terminal cleared.';
    });
  }

  const copyBtn = document.getElementById('btnCopyOutput');
  if (copyBtn) {
    copyBtn.addEventListener('click', () => {
      const terminalOut = document.getElementById('terminalOutputArea');
      if (terminalOut) {
        navigator.clipboard.writeText(terminalOut.innerText);
        const originalText = copyBtn.innerHTML;
        copyBtn.innerHTML = 'Copied!';
        setTimeout(() => copyBtn.innerHTML = originalText, 1500);
      }
    });
  }

  // Deep dive tabs (Explanation, Flow, Viva)
  const deepDiveBtns = document.querySelectorAll('.deep-dive-btn');
  deepDiveBtns.forEach(btn => {
    btn.addEventListener('click', () => {
      deepDiveBtns.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      const target = btn.getAttribute('data-target');
      document.querySelectorAll('.deep-dive-content').forEach(c => c.classList.remove('active'));
      const activeContent = document.getElementById(target);
      if (activeContent) activeContent.classList.add('active');
    });
  });
}

function displayTerminalContent() {
  const terminalOut = document.getElementById('terminalOutputArea');
  if (!terminalOut) return;

  if (!State.lastExecution) {
    terminalOut.innerHTML = 'No program has been executed yet.';
    return;
  }

  const res = State.lastExecution;

  if (State.activeTerminalTab === 'stdout') {
    terminalOut.innerHTML = ansiToHtml(res.stdout || '<span style="color: var(--text-dim);">[No stdout produced]</span>');
  } else if (State.activeTerminalTab === 'stderr') {
    terminalOut.innerHTML = ansiToHtml(res.stderr || '<span style="color: #34d399;">[Stderr is clean - No runtime errors]</span>');
  } else if (State.activeTerminalTab === 'compile') {
    terminalOut.innerHTML = ansiToHtml(res.compile_output || 'Compilation succeeded.');
  } else if (State.activeTerminalTab === 'strace') {
    terminalOut.innerHTML = ansiToHtml(res.strace_output || '<span style="color: var(--text-dim);">Strace audit disabled. Toggle "Trace Kernel Syscalls (strace)" and re-run.</span>');
  }
}

// Execution Actions (Run, Strace toggle, Reset Code)
function setupExecutionActions() {
  const runBtn = document.getElementById('btnRunCode');
  const straceBtn = document.getElementById('btnToggleStrace');
  const resetBtn = document.getElementById('btnResetCode');

  if (straceBtn) {
    straceBtn.addEventListener('click', () => {
      State.useStrace = !State.useStrace;
      straceBtn.classList.toggle('active', State.useStrace);
      if (State.useStrace) {
        straceBtn.innerHTML = '✓ strace Enabled';
      } else {
        straceBtn.innerHTML = '⚡ Trace Syscalls (strace)';
      }
    });
  }

  if (resetBtn) {
    resetBtn.addEventListener('click', () => {
      const textarea = document.getElementById('cCodeTextarea');
      if (textarea && State.currentCode) {
        textarea.value = State.currentCode;
      }
    });
  }

  if (runBtn) {
    runBtn.addEventListener('click', async () => {
      if (State.isLoading || !State.currentDemo) return;
      await executeCurrentDemo();
    });
  }
}

// Execute C Program on Ubuntu Linux
async function executeCurrentDemo() {
  const runBtn = document.getElementById('btnRunCode');
  const textarea = document.getElementById('cCodeTextarea');
  const argsInput = document.getElementById('cmdArgsInput');
  const stdinInput = document.getElementById('stdinInput');
  const statusBadge = document.getElementById('terminalStatusBadge');
  const timerBadge = document.getElementById('terminalTimerBadge');
  const terminalOut = document.getElementById('terminalOutputArea');

  State.isLoading = true;
  runBtn.innerHTML = '<span class="spinner"></span> Compiling on Ubuntu...';
  runBtn.disabled = true;

  if (statusBadge) {
    statusBadge.textContent = 'RUNNING';
    statusBadge.className = 'status-badge';
    statusBadge.style.background = 'rgba(6, 182, 212, 0.15)';
    statusBadge.style.color = '#38bdf8';
  }

  const customCode = textarea ? textarea.value : '';
  const args = argsInput ? argsInput.value : '';
  const stdin = stdinInput ? stdinInput.value : '';

  try {
    const payload = {
      demo_id: State.currentDemo.id,
      custom_code: (customCode !== State.currentCode) ? customCode : null,
      args: args,
      stdin: stdin,
      use_strace: State.useStrace
    };

    const res = await fetch('/api/execute', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });

    const data = await res.json();
    State.lastExecution = data;

    // Update Timer & Status
    if (timerBadge) timerBadge.textContent = `${data.execution_time_ms} ms`;
    if (statusBadge) {
      if (data.success && data.exit_code === 0) {
        statusBadge.textContent = `EXIT: ${data.exit_code} (OK)`;
        statusBadge.className = 'status-badge badge-success';
      } else {
        statusBadge.textContent = `EXIT: ${data.exit_code} (FAILED)`;
        statusBadge.className = 'status-badge badge-error';
      }
    }

    // Auto-switch terminal tab if compilation failed or if strace was requested
    if (!data.success && data.phase === 'compilation') {
      State.activeTerminalTab = 'compile';
      document.querySelectorAll('.panel-tab[data-term-tab]').forEach(t => {
        t.classList.toggle('active', t.getAttribute('data-term-tab') === 'compile');
      });
    } else if (State.useStrace && data.strace_output) {
      State.activeTerminalTab = 'strace';
      document.querySelectorAll('.panel-tab[data-term-tab]').forEach(t => {
        t.classList.toggle('active', t.getAttribute('data-term-tab') === 'strace');
      });
    } else {
      State.activeTerminalTab = 'stdout';
      document.querySelectorAll('.panel-tab[data-term-tab]').forEach(t => {
        t.classList.toggle('active', t.getAttribute('data-term-tab') === 'stdout');
      });
    }

    displayTerminalContent();

  } catch (err) {
    if (terminalOut) {
      terminalOut.innerHTML = `<span style="color: #fb7185;">Network or Server Error: ${err.message}</span>`;
    }
  } finally {
    State.isLoading = false;
    runBtn.innerHTML = '▶ Compile & Run on Ubuntu (GCC)';
    runBtn.disabled = false;
  }
}

// Render System Call Explorer Table
function renderExplorerTable(list) {
  const tbody = document.getElementById('explorerTableBody');
  if (!tbody) return;

  tbody.innerHTML = '';
  list.forEach(item => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td><span class="syscall-pill">${item.name}()</span></td>
      <td><span class="co-badge">${item.co}</span></td>
      <td>${item.module_title}</td>
      <td>
        ${item.related_demos.map(d => `
          <button class="btn btn-secondary" style="padding: 0.25rem 0.65rem; font-size: 0.75rem; margin-right: 0.35rem;" onclick="launchFromExplorer('${d.id}')">
            ${d.title}
          </button>
        `).join('')}
      </td>
    `;
    tbody.appendChild(tr);
  });
}

function setupExplorerSearch() {
  const searchInput = document.getElementById('explorerSearchInput');
  if (!searchInput) return;

  searchInput.addEventListener('input', (e) => {
    const q = e.target.value.toLowerCase().trim();
    if (!q) {
      renderExplorerTable(State.explorerSyscalls);
      return;
    }
    const filtered = State.explorerSyscalls.filter(item => 
      item.name.toLowerCase().includes(q) ||
      item.co.toLowerCase().includes(q) ||
      item.module_title.toLowerCase().includes(q) ||
      item.related_demos.some(d => d.title.toLowerCase().includes(q))
    );
    renderExplorerTable(filtered);
  });
}

window.launchFromExplorer = function(demoId) {
  selectDemo(demoId, true);
};

// Viva Examination Bank View
function renderVivaModuleTabs() {
  const container = document.getElementById('vivaAllModulesContainer');
  if (!container) return;

  let html = '';
  State.modules.forEach(mod => {
    html += `
      <div style="margin-bottom: 2rem;">
        <div style="display: flex; align-items: center; gap: 0.75rem; margin-bottom: 1rem;">
          <span class="co-badge" style="font-size: 1rem;">${mod.code}</span>
          <h3 style="font-size: 1.25rem; font-weight: 700; color: #fff;">${mod.title}</h3>
        </div>
        <div class="viva-list">
    `;

    mod.demos.forEach(demo => {
      if (demo.viva_questions && demo.viva_questions.length > 0) {
        demo.viva_questions.forEach((vq, i) => {
          html += `
            <div class="viva-card">
              <div class="viva-question" onclick="toggleVivaCard(this)">
                <span><strong>[${demo.title}]</strong> ${vq.q}</span>
                <span class="viva-chevron">▼</span>
              </div>
              <div class="viva-answer">
                <p>${vq.a}</p>
                ${vq.tip ? `<div class="viva-tip"><strong>Viva Tip:</strong> ${vq.tip}</div>` : ''}
              </div>
            </div>
          `;
        });
      }
    });

    html += `
        </div>
      </div>
    `;
  });

  container.innerHTML = html;
}
