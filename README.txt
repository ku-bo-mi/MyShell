<h1>My Shell</h1>

<p>My Shell is a simple Unix-like shell implemented in C, designed for practicing system programming skills. This project showcases my ability to work with processes, forks, and system calls while providing users with a basic command-line interface.</p>

<h2>Features</h2>

<ul>
<li><strong>Basic Shell Operations:</strong> My Shell allows users to enter shell commands just like a standard Unix shell.</li>
<li><strong>Command Execution:</strong> It creates child processes to execute the entered shell commands and waits for them to complete.</li>
<li><strong>Redirection:</strong> Users can redirect standard input and output using <code>&lt;</code> and <code>&gt;</code> respectively.</li>
<li><strong>Pipes:</strong> My Shell supports pipes (<code>|</code>) to enable users to chain multiple commands together, similar to how they work in Unix shells.</li>
</ul>

<h2>Getting Started</h2>

<ol>
<li>Clone this repository to your local machine:</li>
</ol>

<pre><code>git clone https://github.com/yourusername/my-shell.git
</code></pre>

<ol start="2">
<li>Compile the code:</li>
</ol>

<pre><code>make
</code></pre>

<ol start="3">
<li>Run the shell:</li>
</ol>

<pre><code>./my-shell
</code></pre>

<h2>Usage</h2>

<ol>
<li>Start the shell by running <code>./my-shell</code>.</li>
<li>Enter commands just like you would in a regular Unix shell.</li>
<li>Use <code>&lt;</code> to redirect standard input from a file:</li>
</ol>

<pre><code>cat &lt; input.txt
</code></pre>

<ol start="4">
<li>Use <code>&gt;</code> to redirect standard output to a file:</li>
</ol>

<pre><code>ls &gt; output.txt
</code></pre>

<ol start="5">
<li>Use <code>|</code> to pipe the output of one command into another:</li>
</ol>

<pre><code>ls | grep keyword
</code></pre>

<ol start="6">
<li>Press <code>Ctrl + C</code> to interrupt a running command.</li>
<li>To exit the shell, simply type <code>exit</code> or use <code>Ctrl + D</code>.</li>
</ol>

<h2>Author</h2>

<ul>
<li><a href="https://github.com/yourusername">Kubomi</a></li>
</ul>
