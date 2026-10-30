/**
 * diagrams.js
 * Interactive SVG and Canvas visualizers for Operating System internals:
 * - System Call Traps (User Mode Ring 3 -> Kernel Mode Ring 0)
 * - Process Hierarchy Trees (fork / exec / wait)
 * - Virtual Memory Address Translation & TLB Model
 * - Inter-Process Pipe Ring Buffers
 * - Thread Mutex Synchronization & Critical Section State
 */

const DiagramVisualizer = {
  // Render Step-by-Step Execution Sequence
  renderFlowChain: function(steps, container) {
    if (!container) return;
    if (!steps || steps.length === 0) {
      container.innerHTML = '<div style="color: var(--text-dim); text-align: center;">No execution flow steps specified.</div>';
      return;
    }

    let html = '<div class="flow-step-chain">';
    steps.forEach((step, idx) => {
      const isKernel = step.actor.toLowerCase().includes('kernel') || step.actor.toLowerCase().includes('hardware') || step.actor.toLowerCase().includes('ring 0');
      const actorColor = isKernel ? 'var(--accent-purple)' : 'var(--accent-cyan)';
      const actorBadge = isKernel ? 'Ring 0 [Kernel]' : 'Ring 3 [User]';

      html += `
        <div class="flow-step-card" style="border-left: 3px solid ${actorColor};">
          <div class="flow-step-num" style="background: ${actorColor}">${step.step || (idx + 1)}</div>
          <div class="flow-step-actor" style="color: ${actorColor}">
            ${step.actor}
            <div style="font-size: 0.68rem; color: var(--text-dim); font-weight: normal;">${actorBadge}</div>
          </div>
          <div class="flow-step-action">${step.action}</div>
        </div>
      `;
    });
    html += '</div>';
    container.innerHTML = html;
  },

  // Interactive Virtual Memory Paging & Address Translation Calculator
  renderMemoryCalculator: function(containerId, initialHex = "0x7ffd1234abcd") {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
      <div class="vm-sandbox">
        <div class="section-title" style="font-size: 1.15rem; margin-bottom: 0.5rem;">
          <span>Virtual Address Translation Engine (4 KB Paging)</span>
        </div>
        <p style="font-size: 0.85rem; color: var(--text-muted); margin-bottom: 1rem;">
          Enter any 64-bit or 32-bit virtual memory address to observe real-time bitmask splitting (VPN + Offset) and Physical RAM calculation.
        </p>

        <div class="vm-input-row">
          <div class="input-field" style="max-width: 320px;">
            <label>Virtual Address (Hex or Decimal)</label>
            <input type="text" id="vmAddressInput" value="${initialHex}" placeholder="e.g. 0x7ffd1234abcd">
          </div>
          <button class="btn btn-primary" id="btnTranslateVM" style="align-self: flex-end;">Translate Address</button>
        </div>

        <div class="bit-breakdown" id="vmResultsContainer">
          <!-- Dynamically populated -->
        </div>
      </div>
    `;

    const input = document.getElementById("vmAddressInput");
    const btn = document.getElementById("btnTranslateVM");

    const calculate = () => {
      let raw = input.value.trim();
      let addr = 0n;
      try {
        if (raw.startsWith("0x") || raw.startsWith("0X")) {
          addr = BigInt(raw);
        } else {
          addr = BigInt(raw);
        }
      } catch (e) {
        addr = 0x7ffd1234abcdn;
        input.value = "0x7ffd1234abcd";
      }

      const offsetMask = 0xFFFn; // Lower 12 bits
      const offset = addr & offsetMask;
      const vpn = addr >> 12n;
      const simulatedPfn = (vpn * 13n + 0x2A1n) & 0xFFFFFFn;
      const physicalAddr = (simulatedPfn << 12n) | offset;

      const resultsDiv = document.getElementById("vmResultsContainer");
      resultsDiv.innerHTML = `
        <div class="bit-box">
          <h4>Input Virtual Address</h4>
          <p>0x${addr.toString(16).padStart(12, '0')}</p>
          <div style="font-size: 0.72rem; color: var(--text-dim); margin-top: 0.2rem;">Full 64-bit Process Pointer</div>
        </div>
        <div class="bit-box" style="border-color: rgba(6, 182, 212, 0.4);">
          <h4 style="color: var(--accent-cyan);">Virtual Page Number (VPN)</h4>
          <p style="color: var(--accent-cyan);">0x${vpn.toString(16)}</p>
          <div style="font-size: 0.72rem; color: var(--text-dim); margin-top: 0.2rem;">Upper bits (Page Table Index)</div>
        </div>
        <div class="bit-box" style="border-color: rgba(139, 92, 246, 0.4);">
          <h4 style="color: var(--accent-purple);">Page Offset (12 bits)</h4>
          <p style="color: var(--accent-purple);">0x${offset.toString(16).padStart(3, '0')} (${offset} B)</p>
          <div style="font-size: 0.72rem; color: var(--text-dim); margin-top: 0.2rem;">Byte location in 4096-byte frame</div>
        </div>
        <div class="bit-box" style="border-color: rgba(16, 185, 129, 0.4);">
          <h4 style="color: var(--accent-emerald);">Mapped Physical RAM Frame</h4>
          <p style="color: var(--accent-emerald);">0x${physicalAddr.toString(16).padStart(12, '0')}</p>
          <div style="font-size: 0.72rem; color: var(--text-dim); margin-top: 0.2rem;">(PFN &lt;&lt; 12) | Offset</div>
        </div>
      `;
    };

    btn.addEventListener("click", calculate);
    input.addEventListener("keydown", (e) => {
      if (e.key === "Enter") calculate();
    });
    calculate();
  },

  // Interactive Process Tree Visualizer
  renderProcessTreeSVG: function(container) {
    if (!container) return;
    container.innerHTML = `
      <div style="width: 100%; display: flex; flex-direction: column; align-items: center; gap: 1rem;">
        <svg width="100%" height="220" viewBox="0 0 600 220" style="max-width: 650px;">
          <defs>
            <linearGradient id="gradRoot" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#0284c7" />
              <stop offset="100%" stop-color="#0369a1" />
            </linearGradient>
            <linearGradient id="gradChild" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#8b5cf6" />
              <stop offset="100%" stop-color="#6d28d9" />
            </linearGradient>
            <linearGradient id="gradLeaf" x1="0%" y1="0%" x2="100%" y2="100%">
              <stop offset="0%" stop-color="#10b981" />
              <stop offset="100%" stop-color="#047857" />
            </linearGradient>
          </defs>

          <!-- Connecting Lines -->
          <line x1="300" y1="50" x2="180" y2="120" stroke="rgba(56, 189, 248, 0.4)" stroke-width="2" stroke-dasharray="4" />
          <line x1="300" y1="50" x2="420" y2="120" stroke="rgba(56, 189, 248, 0.4)" stroke-width="2" stroke-dasharray="4" />
          <line x1="180" y1="140" x2="180" y2="180" stroke="rgba(16, 185, 129, 0.4)" stroke-width="2" />

          <!-- Root Node -->
          <rect x="220" y="20" width="160" height="42" rx="8" fill="url(#gradRoot)" filter="drop-shadow(0 4px 10px rgba(2,132,199,0.4))" />
          <text x="300" y="46" font-family="'Outfit', sans-serif" font-size="13" font-weight="bold" fill="#ffffff" text-anchor="middle">Parent (PID: 1042)</text>

          <!-- Child 1 Node -->
          <rect x="100" y="105" width="160" height="42" rx="8" fill="url(#gradChild)" filter="drop-shadow(0 4px 10px rgba(139,92,246,0.4))" />
          <text x="180" y="131" font-family="'Outfit', sans-serif" font-size="13" font-weight="bold" fill="#ffffff" text-anchor="middle">Child 1 (PID: 1043)</text>

          <!-- Child 2 Node -->
          <rect x="340" y="105" width="160" height="42" rx="8" fill="url(#gradChild)" filter="drop-shadow(0 4px 10px rgba(139,92,246,0.4))" />
          <text x="420" y="131" font-family="'Outfit', sans-serif" font-size="13" font-weight="bold" fill="#ffffff" text-anchor="middle">Child 2 (PID: 1044)</text>

          <!-- Grandchild Node -->
          <rect x="100" y="170" width="160" height="38" rx="8" fill="url(#gradLeaf)" />
          <text x="180" y="194" font-family="'Outfit', sans-serif" font-size="12" font-weight="bold" fill="#ffffff" text-anchor="middle">Grandchild (PID: 1045)</text>
        </svg>
        <div style="font-size: 0.8rem; color: var(--text-dim); text-align: center;">
          In Linux, processes form an acyclic tree starting from systemd/init. Each parent must invoke waitpid() to avoid zombie leaks.
        </div>
      </div>
    `;
  }
};
