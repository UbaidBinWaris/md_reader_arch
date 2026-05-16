# 🎯 Interview Preparation Guide — Ubaid Bin Waris
> Full Stack Engineer | Tailored Q&A Based on Resume

---

## 📌 TABLE OF CONTENTS
1. [HTML & CSS](#1-html--css)
2. [JavaScript & TypeScript](#2-javascript--typescript)
3. [React.js & Next.js](#3-reactjs--nextjs)
4. [Tailwind CSS](#4-tailwind-css)
5. [Node.js & Express.js](#5-nodejs--expressjs)
6. [REST APIs](#6-rest-apis)
7. [Security — JWT, RBAC, CORS, Rate Limiting](#7-security--jwt-rbac-cors-rate-limiting)
8. [Databases — PostgreSQL, MongoDB, MySQL, Redis](#8-databases--postgresql-mongodb-mysql-redis)
9. [Supabase](#9-supabase)
10. [Spring Boot & Java](#10-spring-boot--java)
11. [Cloud & AWS](#11-cloud--aws)
12. [DevOps — Docker, CI/CD, GitHub Actions, Nginx, PM2](#12-devops--docker-cicd-github-actions-nginx-pm2)
13. [Automation — n8n, Webhooks, Cron Jobs](#13-automation--n8n-webhooks-cron-jobs)
14. [System Design](#14-system-design)
15. [Project-Based Questions](#15-project-based-questions)
16. [Behavioral & HR Questions](#16-behavioral--hr-questions)

---

## DIFFICULTY LEGEND
| Level | Label |
|-------|-------|
| 🟢 | Beginner |
| 🟡 | Intermediate |
| 🔴 | Advanced |
| 🔥 | Expert |

---

## 📖 KEY TERMINOLOGY GLOSSARY
> Definitions for important terms used throughout this guide.

| Term | Definition |
|------|-----------|
| **REST API** | Representational State Transfer — an architectural style for networked apps using HTTP methods (GET, POST, PUT, DELETE) |
| **RBAC** | Role-Based Access Control — restricting system access based on assigned roles |
| **JWT** | JSON Web Token — a compact, self-contained token for securely transmitting information between parties |
| **CORS** | Cross-Origin Resource Sharing — a browser security mechanism that controls cross-domain HTTP requests |
| **CI/CD** | Continuous Integration/Continuous Deployment — automating code testing and deployment pipelines |
| **ORM** | Object-Relational Mapping — a technique that lets you interact with databases using your programming language |
| **SSR** | Server-Side Rendering — rendering HTML on the server before sending to the client |
| **CSR** | Client-Side Rendering — rendering HTML in the browser using JavaScript |
| **ISR** | Incremental Static Regeneration — regenerating static pages on-demand in Next.js |
| **Middleware** | Software layer between request and response pipeline |
| **Rate Limiting** | Restricting the number of API requests a client can make in a given time window |
| **VPS** | Virtual Private Server — a virtualized server running its own OS |
| **Reverse Proxy** | A server (like Nginx) that forwards client requests to backend servers |
| **PM2** | Process Manager 2 — a production process manager for Node.js apps |
| **Webhook** | An HTTP callback triggered by a specific event in another system |
| **Cron Job** | A time-based job scheduler running automated tasks at specified intervals |
| **n8n** | An open-source workflow automation tool for connecting apps and services |
| **Docker** | A containerization platform that packages applications with all dependencies |
| **Docker Compose** | Tool for defining and running multi-container Docker applications |
| **SSL/TLS** | Secure Sockets Layer / Transport Layer Security — cryptographic protocols for secure communication |
| **FFmpeg** | A multimedia framework for recording, converting, and streaming audio/video |
| **Puppeteer** | A Node.js library for controlling headless Chrome/Chromium |
| **Supabase** | An open-source Firebase alternative built on PostgreSQL |
| **Redis** | An in-memory data structure store used as a database, cache, and message broker |
| **Normalization** | Organizing database tables to reduce redundancy and improve data integrity |
| **Index** | A database structure that speeds up data retrieval operations |
| **Sharding** | Horizontal partitioning of data across multiple database instances |
| **CAP Theorem** | States that a distributed system can only guarantee 2 of 3: Consistency, Availability, Partition Tolerance |
| **MVC** | Model-View-Controller — a software design pattern separating concerns |

---

## 1. HTML & CSS

### 🟢 Q1. What is the difference between `<div>` and `<span>`?
**Answer:**
`<div>` is a **block-level** element — it takes up the full width of its parent and starts on a new line. It is used for larger structural layout sections. `<span>` is an **inline** element — it only takes up as much width as its content and does not start on a new line. It is used for styling small pieces of text or elements within a block.

**Example:**
```html
<div>This takes the full row</div>
<span>This</span> sits inline with text.
```

---

### 🟢 Q2. What is the CSS Box Model?
**Answer:**
The **CSS Box Model** describes how every HTML element is represented as a rectangular box consisting of:
1. **Content** — the actual text or image
2. **Padding** — transparent space between content and border
3. **Border** — the edge around padding
4. **Margin** — transparent space outside the border, separating it from other elements

`box-sizing: border-box` makes padding and border included in the element's total width/height — the modern preferred approach.

---

### 🟢 Q3. What is the difference between `position: relative`, `absolute`, `fixed`, and `sticky`?
**Answer:**
- **`relative`** — positioned relative to its normal flow position; does not leave the document flow
- **`absolute`** — removed from the document flow; positioned relative to the nearest positioned ancestor (`position: relative/absolute/fixed`)
- **`fixed`** — removed from flow; positioned relative to the **viewport** — stays fixed even on scroll
- **`sticky`** — hybrid of relative and fixed; sticks to a position once the user scrolls past a threshold

---

### 🟡 Q4. Explain Flexbox and its main properties.
**Answer:**
**Flexbox** (Flexible Box Layout) is a CSS layout model designed for arranging items in a one-dimensional row or column.

**Container properties:**
- `display: flex` — activates flexbox
- `flex-direction: row | column` — sets main axis
- `justify-content` — aligns items along the main axis (start, center, space-between, space-around)
- `align-items` — aligns items along the cross axis
- `flex-wrap` — allows items to wrap to next line
- `gap` — sets spacing between flex items

**Item properties:**
- `flex-grow` — how much an item grows relative to siblings
- `flex-shrink` — how much it shrinks
- `flex-basis` — default size before growing/shrinking
- `align-self` — overrides align-items for a single item

---

### 🟡 Q5. What is CSS Grid and how is it different from Flexbox?
**Answer:**
**CSS Grid** is a two-dimensional layout system (rows AND columns simultaneously), while **Flexbox** is one-dimensional (either row or column). Use Grid for overall page layout (header, sidebar, main, footer) and Flexbox for component-level layout (navigation bar, card items). Grid gives more explicit control over placement; Flexbox is better for dynamic content sizing.

---

### 🟡 Q6. What are CSS custom properties (variables) and why are they useful?
**Answer:**
CSS custom properties (variables) are defined with `--` prefix and accessed via `var()`. They enable consistent theming across a stylesheet and are dynamically updatable via JavaScript.

```css
:root {
  --primary-color: #3b82f6;
  --font-size-base: 16px;
}
button {
  background: var(--primary-color);
}
```
**Benefits:** Centralized value management, easy theme switching, reduce repetition, support runtime changes.

---

### 🟡 Q7. What is responsive design and how do you implement it?
**Answer:**
**Responsive design** ensures a website adapts its layout and content to different screen sizes (mobile, tablet, desktop). Implementation strategies:
1. **Media Queries** — `@media (max-width: 768px) { ... }`
2. **Fluid grids** — using percentage-based widths
3. **Flexible images** — `max-width: 100%`
4. **Viewport meta tag** — `<meta name="viewport" content="width=device-width, initial-scale=1">`
5. **CSS Grid / Flexbox** — inherently responsive layout tools
6. **Tailwind CSS** — utility-based responsive prefixes (`md:`, `lg:`)

---

### 🔴 Q8. What is Critical Rendering Path and how can you optimize it?
**Answer:**
The **Critical Rendering Path (CRP)** is the sequence of steps a browser takes to render a page:
1. Parse HTML → Build **DOM**
2. Parse CSS → Build **CSSOM**
3. Combine DOM + CSSOM → **Render Tree**
4. **Layout** — calculate geometry
5. **Paint** — draw pixels

**Optimizations:**
- Minify CSS and JS
- Defer non-critical JS (`defer` or `async` attributes)
- Inline critical CSS in `<head>`
- Eliminate render-blocking resources
- Use lazy loading for images
- Compress assets (gzip/Brotli)

---

### 🔴 Q9. What is the difference between `display: none`, `visibility: hidden`, and `opacity: 0`?
**Answer:**
| Property | Space Occupied | Events | Accessibility |
|----------|---------------|--------|---------------|
| `display: none` | No | No | Hidden from screen readers |
| `visibility: hidden` | Yes | No | Hidden from screen readers |
| `opacity: 0` | Yes | Yes | Still readable by screen readers |

Use `display: none` to completely remove. Use `opacity: 0` if you still need click events. `visibility: hidden` is rarely preferred.

---

## 2. JavaScript & TypeScript

### 🟢 Q10. What is the difference between `var`, `let`, and `const`?
**Answer:**
- **`var`** — function-scoped, hoisted to the top of its scope, can be re-declared and updated. Can cause bugs due to hoisting.
- **`let`** — block-scoped, not re-declarable, can be updated. The modern safe alternative to `var`.
- **`const`** — block-scoped, cannot be re-assigned after declaration. Objects/arrays declared with `const` can still have their properties mutated.

**Terminology — Hoisting:** JavaScript moves `var` declarations to the top of their scope during compilation, initializing them as `undefined`.

---

### 🟢 Q11. What are arrow functions and how are they different from regular functions?
**Answer:**
Arrow functions use `=>` syntax and differ in key ways:
1. **`this` binding** — Arrow functions do NOT have their own `this`. They inherit `this` from the enclosing lexical scope. Regular functions have their own `this`.
2. **Syntax** — More concise: `const add = (a, b) => a + b`
3. **`arguments` object** — Not available in arrow functions
4. **Cannot be used as constructors** — `new ArrowFn()` throws an error

```javascript
// Regular function
function greet() { return `Hello ${this.name}`; }
// Arrow function
const greet = () => `Hello ${this.name}`; // 'this' is lexical
```

---

### 🟡 Q12. Explain the Event Loop, Call Stack, and Microtask Queue in JavaScript.
**Answer:**
JavaScript is **single-threaded** but handles asynchronous operations via the **Event Loop**.

- **Call Stack** — LIFO data structure where function calls are tracked. Synchronous code runs here.
- **Web APIs** — Browser APIs (setTimeout, fetch) handle async tasks outside the main thread.
- **Callback Queue (Macrotask Queue)** — Holds callbacks from `setTimeout`, `setInterval`, DOM events.
- **Microtask Queue** — Holds callbacks from Promises (`.then`, `.catch`) and `MutationObserver`. Has **higher priority** than the callback queue.
- **Event Loop** — Continuously checks: if the Call Stack is empty, first drain the Microtask Queue, then take one task from the Callback Queue.

**Order of execution:**
```
Synchronous → Microtasks (Promises) → Macrotasks (setTimeout)
```

---

### 🟡 Q13. What is a Promise and how does async/await work?
**Answer:**
A **Promise** is an object representing the eventual completion or failure of an asynchronous operation. It has three states:
- **Pending** — initial state
- **Fulfilled** — operation completed successfully
- **Rejected** — operation failed

**async/await** is syntactic sugar over Promises, making async code look synchronous:
```javascript
// Promise chaining
fetch('/api/users').then(res => res.json()).then(data => console.log(data));

// async/await equivalent
async function getUsers() {
  try {
    const res = await fetch('/api/users');
    const data = await res.json();
    console.log(data);
  } catch (err) {
    console.error(err);
  }
}
```
`await` pauses execution inside the `async` function until the Promise resolves, without blocking the main thread.

---

### 🟡 Q14. What is closure in JavaScript?
**Answer:**
A **closure** is a function that retains access to its outer (enclosing) scope even after the outer function has returned. The inner function "closes over" the variables of the outer scope.

```javascript
function counter() {
  let count = 0;
  return function increment() {
    count++;
    return count;
  };
}
const myCounter = counter();
myCounter(); // 1
myCounter(); // 2  — count is still accessible
```

**Real-world use:** Data encapsulation, memoization, event handlers, factory functions.

---

### 🟡 Q15. What is the difference between `==` and `===` in JavaScript?
**Answer:**
- **`==` (Loose equality)** — compares values after **type coercion** (converting types before comparison). `"5" == 5` → `true`
- **`===` (Strict equality)** — compares values AND types, **no coercion**. `"5" === 5` → `false`

Always prefer `===` in production code to avoid unpredictable type coercion bugs.

---

### 🟡 Q16. Explain `map()`, `filter()`, and `reduce()` with examples.
**Answer:**
All three are **higher-order array methods** (they take a function as an argument).

- **`map()`** — transforms each element, returns new array of same length
```javascript
[1, 2, 3].map(x => x * 2); // [2, 4, 6]
```
- **`filter()`** — keeps elements that satisfy the condition, returns new array (possibly shorter)
```javascript
[1, 2, 3, 4].filter(x => x % 2 === 0); // [2, 4]
```
- **`reduce()`** — accumulates all elements into a single value
```javascript
[1, 2, 3, 4].reduce((acc, cur) => acc + cur, 0); // 10
```

---

### 🟡 Q17. What is TypeScript and why would you use it over JavaScript?
**Answer:**
**TypeScript** is a statically typed superset of JavaScript that compiles to plain JavaScript. It adds optional static typing, interfaces, enums, generics, and compile-time error detection.

**Benefits:**
- Catch type errors at compile time, not runtime
- Better IDE autocomplete and IntelliSense
- Self-documenting code via type annotations
- Easier refactoring in large codebases
- Interfaces and generics for reusable, type-safe abstractions

```typescript
interface User {
  id: number;
  name: string;
  email: string;
}

function getUser(id: number): Promise<User> { ... }
```

---

### 🔴 Q18. What are TypeScript Generics and when would you use them?
**Answer:**
**Generics** allow you to write reusable, type-safe components that work with multiple types rather than hardcoding a specific type.

```typescript
// Without generics — loses type info
function identity(arg: any): any { return arg; }

// With generics — type-safe
function identity<T>(arg: T): T { return arg; }

// Usage
identity<string>("hello"); // returns string
identity<number>(42);       // returns number
```

**Real-world use:** Generic API response wrappers, reusable hooks, data structures.

```typescript
interface ApiResponse<T> {
  data: T;
  status: number;
  message: string;
}
```

---

### 🔴 Q19. What is the difference between `call()`, `apply()`, and `bind()` in JavaScript?
**Answer:**
All three methods control the value of `this` in a function:

- **`call(context, arg1, arg2)`** — invokes the function immediately with a given `this` and individual arguments
- **`apply(context, [arg1, arg2])`** — same as `call` but arguments passed as an **array**
- **`bind(context)`** — returns a **new function** with `this` permanently bound, does not invoke immediately

```javascript
function greet(greeting) { return `${greeting}, ${this.name}`; }
const user = { name: 'Ubaid' };

greet.call(user, 'Hello');       // "Hello, Ubaid" — immediate
greet.apply(user, ['Hello']);    // "Hello, Ubaid" — immediate
const bound = greet.bind(user);
bound('Hello');                  // "Hello, Ubaid" — deferred
```

---

### 🔥 Q20. Explain the JavaScript Prototype Chain and prototypal inheritance.
**Answer:**
Every JavaScript object has an internal `[[Prototype]]` property pointing to another object (its prototype). When you access a property on an object, JavaScript first looks at the object itself, then walks up the **prototype chain** until it finds the property or reaches `null`.

```javascript
function Animal(name) { this.name = name; }
Animal.prototype.speak = function() { return `${this.name} speaks`; };

const dog = new Animal('Rex');
dog.speak(); // Found on prototype, not dog itself
```

**ES6 Classes** are syntactic sugar over this prototype system. When you use `class`, JavaScript still uses prototypes under the hood.

**Key concepts:**
- `Object.getPrototypeOf(obj)` — access prototype
- `hasOwnProperty()` — check if property is on the object itself (not inherited)
- `__proto__` — deprecated reference to prototype

---

## 3. React.js & Next.js

### 🟢 Q21. What is React and what problem does it solve?
**Answer:**
**React** is a JavaScript library by Meta for building user interfaces through **component-based architecture**. It solves the problem of efficiently updating the DOM by using a **Virtual DOM** — an in-memory representation of the real DOM. When state changes, React diffs the old and new Virtual DOM (reconciliation) and applies only the minimal set of changes to the real DOM, making updates highly performant.

**Key concepts:** Component, Props, State, Virtual DOM, JSX

---

### 🟢 Q22. What is the difference between `props` and `state` in React?
**Answer:**
- **Props** — short for "properties". Data passed **from parent to child** component. They are **read-only** (immutable) — a component cannot modify its own props.
- **State** — data managed **within** a component. It is **mutable** — when state changes, React re-renders the component. Managed via `useState` hook.

```jsx
// Props
function Greeting({ name }) { return <h1>Hello, {name}</h1>; }

// State
function Counter() {
  const [count, setCount] = useState(0);
  return <button onClick={() => setCount(count + 1)}>{count}</button>;
}
```

---

### 🟡 Q23. Explain the most important React Hooks: `useState`, `useEffect`, `useContext`, `useRef`, `useMemo`, `useCallback`.
**Answer:**

- **`useState(initialValue)`** — declares a state variable and setter function. Re-renders component on change.
- **`useEffect(fn, deps)`** — runs side effects (API calls, subscriptions, timers) after render. `deps` array controls when it runs.
  - `[]` — run once on mount
  - `[value]` — run when value changes
  - No array — run on every render
- **`useContext(Context)`** — consumes a context value without prop drilling.
- **`useRef(initialValue)`** — holds a mutable reference that persists across renders WITHOUT triggering re-render. Used for DOM access and storing previous values.
- **`useMemo(fn, deps)`** — memoizes expensive computed values; recomputes only when deps change. Prevents unnecessary recalculations.
- **`useCallback(fn, deps)`** — memoizes a function reference; prevents child re-renders when passing callbacks as props.

**Terminology — Memoization:** Caching the result of a function call so repeated calls with same inputs return cached result.

---

### 🟡 Q24. What is the Virtual DOM and how does React's reconciliation work?
**Answer:**
The **Virtual DOM (VDOM)** is a lightweight, in-memory JavaScript object tree that mirrors the real DOM structure. When state or props change:

1. React creates a **new Virtual DOM tree**
2. **Diffing algorithm** compares old VDOM vs new VDOM (O(n) complexity using heuristics)
3. React calculates the **minimal set of changes** (patches)
4. Only those patches are applied to the **real DOM** (batched updates)

**Reconciliation Heuristics:**
- Elements of different types produce different trees
- Keys help React identify which list items changed/moved/added

**Terminology — Diffing:** The process of comparing two VDOM trees to find differences.

---

### 🟡 Q25. What is the difference between controlled and uncontrolled components?
**Answer:**
- **Controlled Component** — form input's value is controlled by React state. Every state change is handled via event handlers. React is the "single source of truth."
```jsx
const [value, setValue] = useState('');
<input value={value} onChange={e => setValue(e.target.value)} />
```
- **Uncontrolled Component** — form input manages its own state internally. You read the value using `useRef` when needed.
```jsx
const inputRef = useRef();
<input ref={inputRef} />
// Read: inputRef.current.value
```
**Prefer controlled components** for validation, conditional disabling, or when input values affect other UI.

---

### 🟡 Q26. What is Next.js and how is it different from plain React?
**Answer:**
**Next.js** is a React framework by Vercel that adds production-ready features on top of React:

| Feature | React (CRA) | Next.js |
|---------|------------|---------|
| Rendering | CSR only | SSR, SSG, ISR, CSR |
| Routing | Manual (React Router) | File-based routing |
| API Routes | External server needed | Built-in API routes |
| Performance | Manual setup | Automatic code splitting, lazy loading |
| SEO | Poor (client-rendered) | Excellent (server-rendered HTML) |
| Image Optimization | Manual | Built-in `<Image>` component |

---

### 🟡 Q27. Explain the different rendering strategies in Next.js: SSG, SSR, ISR, and CSR.
**Answer:**
- **CSR (Client-Side Rendering)** — HTML sent is empty; JavaScript renders content in browser. Poor SEO, but great for dashboards/authenticated pages.
- **SSG (Static Site Generation)** — HTML generated at **build time**. Super fast, cacheable by CDN. Use for content that rarely changes (blogs, marketing pages). `getStaticProps`.
- **SSR (Server-Side Rendering)** — HTML generated on the **server on each request**. Great for SEO and dynamic, personalized data. Higher server load. `getServerSideProps`.
- **ISR (Incremental Static Regeneration)** — SSG pages that can be **re-generated in background** after a set interval without full rebuild. Best of SSG + SSR.

```javascript
// ISR example — regenerate every 60 seconds
export async function getStaticProps() {
  return { props: { data }, revalidate: 60 };
}
```

---

### 🔴 Q28. What are React Performance optimization techniques?
**Answer:**
1. **`React.memo`** — prevents functional component re-render if props haven't changed
2. **`useMemo`** — memoize expensive calculations
3. **`useCallback`** — memoize function references to avoid child re-renders
4. **Lazy loading / Code splitting** — `React.lazy()` + `Suspense` for dynamic imports
5. **Virtualization** — render only visible list items (react-window, react-virtual)
6. **Avoid inline functions/objects in JSX** — they create new references on every render
7. **Key optimization** — use stable, unique keys in lists (not index)
8. **Debounce/Throttle** expensive event handlers (search inputs, scroll)
9. **Pagination / Infinite scroll** — avoid loading all data at once
10. **Bundle analysis** — use webpack-bundle-analyzer to find bloat

---

### 🔴 Q29. What is React Context and when should you use it vs a state management library?
**Answer:**
**React Context** provides a way to share values across the component tree without prop drilling. It has a `Provider` and `Consumer` (or `useContext` hook).

**Use Context when:**
- Theme (light/dark mode)
- Current authenticated user
- Language/locale settings
- Low-frequency updates

**Avoid Context for:**
- High-frequency state updates (causes all consumers to re-render)
- Complex state logic with many actions

**Use Redux/Zustand/Jotai when:**
- Complex global state with many interactions
- Time-travel debugging needed
- Large team with clear state contracts

**Terminology — Prop Drilling:** Passing props through many intermediate components just to reach a deeply nested component.

---

### 🔥 Q30. Explain Next.js 13+ App Router vs Pages Router. What are Server Components?
**Answer:**
**Pages Router (legacy):** File-based routing in `/pages` directory. Uses `getServerSideProps`, `getStaticProps` for data fetching.

**App Router (Next.js 13+):** New `/app` directory with:
- **React Server Components (RSC)** by default — render on server, send HTML, no JS shipped to client
- **Client Components** — marked with `'use client'`, run in browser
- **Nested layouts** via `layout.js`
- **Server-side data fetching** directly in components using `async/await`

**Server Components advantages:**
- Zero JS bundle size for server components
- Direct database access without API layer
- Automatic code splitting
- Better initial load performance

```jsx
// Server Component (default) — runs on server only
async function UserList() {
  const users = await db.query('SELECT * FROM users'); // direct DB access
  return <ul>{users.map(u => <li>{u.name}</li>)}</ul>;
}
```

---

## 4. Tailwind CSS

### 🟢 Q31. What is Tailwind CSS and how does it differ from traditional CSS frameworks?
**Answer:**
**Tailwind CSS** is a **utility-first CSS framework** — it provides low-level utility classes (e.g., `flex`, `p-4`, `text-blue-500`) that you compose directly in your HTML/JSX. Unlike Bootstrap (component-based), Tailwind doesn't impose predefined UI components — you build custom designs using atomic classes.

**Benefits:**
- No writing custom CSS files
- No naming conventions (no BEM or OOCSS needed)
- Highly customizable via `tailwind.config.js`
- Tiny production bundle (PurgeCSS removes unused classes)
- Responsive and state variants built-in (`hover:`, `md:`, `dark:`)

---

### 🟡 Q32. How does Tailwind handle responsive design?
**Answer:**
Tailwind uses **mobile-first breakpoint prefixes:**

| Prefix | Min Width | Equivalent |
|--------|-----------|------------|
| (none) | 0px | All screens |
| `sm:` | 640px | Small+ |
| `md:` | 768px | Medium+ |
| `lg:` | 1024px | Large+ |
| `xl:` | 1280px | Extra large+ |
| `2xl:` | 1536px | 2x Extra large+ |

```html
<div class="w-full md:w-1/2 lg:w-1/3">
  Responsive grid item
</div>
```
Classes without prefixes apply to all sizes. Prefixed classes override at that breakpoint and above.

---

### 🟡 Q33. How do you customize Tailwind's theme?
**Answer:**
Customize via `tailwind.config.js`:
```javascript
module.exports = {
  theme: {
    extend: {
      colors: {
        brand: '#3b82f6',
        'brand-dark': '#1e40af',
      },
      fontFamily: {
        custom: ['Inter', 'sans-serif'],
      },
      spacing: {
        '18': '4.5rem',
      }
    }
  }
}
```
Use `extend` to **add** to defaults. Replace directly under `theme` to **override** defaults entirely.

---

## 5. Node.js & Express.js

### 🟢 Q34. What is Node.js and why is it used for backend development?
**Answer:**
**Node.js** is a JavaScript runtime built on Chrome's **V8 engine** that allows JavaScript to run outside the browser (server-side). It uses an **event-driven, non-blocking I/O model** making it efficient and scalable for I/O-intensive operations.

**Why Node.js for backend:**
- Same language (JavaScript) for frontend and backend
- Non-blocking I/O — handles thousands of concurrent connections efficiently
- Large npm ecosystem
- Fast for real-time apps (chat, streaming, APIs)
- Lightweight and scalable

**When NOT to use Node.js:** CPU-intensive tasks (heavy computation, image processing) block the event loop.

---

### 🟢 Q35. What is Express.js and what does it provide over plain Node.js?
**Answer:**
**Express.js** is a minimal, unopinionated web framework for Node.js. Plain Node.js `http` module requires you to manually handle routing, request parsing, and middleware. Express provides:
- Simple routing (`app.get()`, `app.post()`, etc.)
- Middleware system (chain functions to process requests)
- Template engine support
- Error handling
- Request/Response helpers
- Router for modular route organization

```javascript
const express = require('express');
const app = express();

app.use(express.json()); // middleware to parse JSON body

app.get('/users', (req, res) => {
  res.json({ users: [] });
});
```

---

### 🟡 Q36. What is Middleware in Express.js? How does the middleware chain work?
**Answer:**
**Middleware** is a function with access to `(req, res, next)`. It can:
- Execute code
- Modify `req`/`res` objects
- End the request-response cycle
- Call `next()` to pass control to the next middleware

```javascript
// Logger middleware
app.use((req, res, next) => {
  console.log(`${req.method} ${req.path}`);
  next(); // MUST call next() or response will hang
});

// Auth middleware
const authenticate = (req, res, next) => {
  const token = req.headers.authorization;
  if (!token) return res.status(401).json({ error: 'Unauthorized' });
  req.user = verifyToken(token);
  next();
};

app.get('/protected', authenticate, (req, res) => {
  res.json({ user: req.user });
});
```

**Middleware types:** Application-level, Router-level, Error-handling, Third-party (cors, helmet, morgan)

---

### 🟡 Q37. How do you handle errors in Express.js?
**Answer:**
Express has a special **error-handling middleware** with 4 parameters `(err, req, res, next)`:

```javascript
// Sync errors — throw them
app.get('/sync', (req, res) => {
  throw new Error('Something broke');
});

// Async errors — pass to next()
app.get('/async', async (req, res, next) => {
  try {
    const data = await fetchData();
    res.json(data);
  } catch (err) {
    next(err); // passes to error handler
  }
});

// Global error handler (define LAST)
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(err.status || 500).json({
    error: err.message || 'Internal Server Error'
  });
});
```

---

### 🔴 Q38. Explain Node.js streams and when you would use them.
**Answer:**
**Streams** are objects that let you read or write data piece-by-piece (chunks) rather than loading everything into memory at once. Essential for large data sets.

**4 Types of streams:**
- **Readable** — data can be read (fs.createReadStream, HTTP request)
- **Writable** — data can be written (fs.createWriteStream, HTTP response)
- **Duplex** — both readable and writable (TCP socket)
- **Transform** — transforms data as it passes through (zlib compression)

```javascript
// Without streams — loads entire file into memory
const data = fs.readFileSync('large-file.csv'); // BAD for large files

// With streams — memory efficient
fs.createReadStream('large-file.csv')
  .pipe(csvParser())
  .pipe(process.stdout);
```

**Terminology — Backpressure:** When a readable stream produces data faster than a writable stream can consume it; streams handle this automatically via `pipe()`.

---

### 🔴 Q39. What is the difference between `process.nextTick()`, `setImmediate()`, and `setTimeout(0)`?
**Answer:**
All schedule callbacks asynchronously but at different phases of the event loop:

- **`process.nextTick(cb)`** — runs **before** the event loop continues to the next phase (before I/O, before Promises). Highest priority.
- **`Promise.resolve().then(cb)`** — Microtask queue, runs after current operation completes, before I/O.
- **`setImmediate(cb)`** — runs in the **check phase** of the event loop, after I/O callbacks.
- **`setTimeout(cb, 0)`** — runs in the **timers phase**, after I/O (similar to setImmediate but with subtle differences).

**Execution order:** `nextTick → Promises → setImmediate ≈ setTimeout(0)`

---

## 6. REST APIs

### 🟢 Q40. What are the main HTTP methods and their use cases?
**Answer:**
| Method | Purpose | Idempotent? | Safe? |
|--------|---------|-------------|-------|
| **GET** | Retrieve resource | ✅ Yes | ✅ Yes |
| **POST** | Create new resource | ❌ No | ❌ No |
| **PUT** | Replace entire resource | ✅ Yes | ❌ No |
| **PATCH** | Partially update resource | ✅ Yes | ❌ No |
| **DELETE** | Delete resource | ✅ Yes | ❌ No |
| **OPTIONS** | Check allowed methods (CORS preflight) | ✅ Yes | ✅ Yes |

**Idempotent:** Making the same request multiple times produces the same result.
**Safe:** Does not modify server state.

---

### 🟢 Q41. What are HTTP status codes? Give examples.
**Answer:**
| Range | Category | Examples |
|-------|---------|---------|
| **2xx** | Success | 200 OK, 201 Created, 204 No Content |
| **3xx** | Redirection | 301 Moved Permanently, 302 Found, 304 Not Modified |
| **4xx** | Client Error | 400 Bad Request, 401 Unauthorized, 403 Forbidden, 404 Not Found, 422 Unprocessable Entity, 429 Too Many Requests |
| **5xx** | Server Error | 500 Internal Server Error, 502 Bad Gateway, 503 Service Unavailable |

---

### 🟡 Q42. What makes a REST API truly "RESTful"? Explain REST constraints.
**Answer:**
REST (Representational State Transfer) has 6 architectural constraints:
1. **Client-Server** — UI and data storage are separated; improves portability and scalability
2. **Stateless** — Each request must contain ALL information needed; server stores NO client session state
3. **Cacheable** — Responses must declare themselves cacheable or non-cacheable
4. **Uniform Interface** — Resources identified by URIs; manipulated through representations; self-descriptive messages
5. **Layered System** — Client can't tell whether it's connected directly to the server or an intermediary (proxy, load balancer)
6. **Code on Demand (optional)** — Server can send executable code (JavaScript) to client

---

### 🟡 Q43. How do you design RESTful URL structures?
**Answer:**
**Best practices for REST URL design:**
```
✅ Use nouns, not verbs
GET  /users          — get all users
GET  /users/42       — get user 42
POST /users          — create user
PUT  /users/42       — update user 42
DELETE /users/42     — delete user 42

✅ Nested resources
GET  /users/42/orders       — get orders for user 42
POST /users/42/orders       — create order for user 42

✅ Filtering, sorting, pagination
GET /users?role=admin&sort=name&page=1&limit=20

❌ Avoid verbs in URLs
❌ /getUsers
❌ /createUser
❌ /users/42/delete
```

---

### 🔴 Q44. What is API versioning and what strategies exist?
**Answer:**
**API versioning** ensures backward compatibility when making breaking changes. Strategies:

1. **URL versioning** (most common): `GET /api/v1/users` → `GET /api/v2/users`
2. **Header versioning**: `Accept: application/vnd.api+json;version=2`
3. **Query parameter**: `GET /api/users?version=2`
4. **Content negotiation**: `Accept: application/vnd.myapp.v2+json`

**Best practice:** URL versioning is most explicit and easiest to debug. Version only when making **breaking changes** (removing fields, changing data types, restructuring responses).

---

## 7. Security — JWT, RBAC, CORS, Rate Limiting

### 🟡 Q45. What is JWT and how does authentication with JWT work?
**Answer:**
**JWT (JSON Web Token)** is a compact, URL-safe token consisting of 3 base64-encoded parts separated by dots:

```
header.payload.signature
```

- **Header** — algorithm and token type: `{ "alg": "HS256", "typ": "JWT" }`
- **Payload** — claims (user data): `{ "sub": "42", "role": "admin", "exp": 1716000000 }`
- **Signature** — `HMACSHA256(base64(header) + "." + base64(payload), secret)`

**Authentication flow:**
1. User logs in with credentials
2. Server verifies, generates JWT signed with secret
3. Client stores JWT (localStorage or httpOnly cookie)
4. Client sends JWT in `Authorization: Bearer <token>` header
5. Server verifies signature on every protected request
6. If valid + not expired → grant access

**Key properties:** Stateless (server needs no session store), self-contained, can be verified without DB lookup.

---

### 🟡 Q46. What are the security risks of JWT and how do you mitigate them?
**Answer:**
| Risk | Mitigation |
|------|-----------|
| Token theft | Short expiry (`exp`), httpOnly cookies, HTTPS |
| Algorithm confusion (`alg: none`) | Explicitly validate algorithm on server |
| No invalidation (stateless) | Short expiry + refresh tokens + token blacklist/allowlist |
| Sensitive data in payload | Never store passwords/secrets in payload (it's base64, not encrypted) |
| Large token size | Keep payload minimal |

**Refresh Token Pattern:** Short-lived access token (15 min) + long-lived refresh token (7 days). Store refresh in httpOnly cookie; issue new access token when it expires.

---

### 🟡 Q47. What is RBAC and how would you implement it?
**Answer:**
**RBAC (Role-Based Access Control)** restricts system access based on user roles. Key concepts:
- **Roles** — Admin, User, Moderator, Editor
- **Permissions** — read, write, delete, manage
- **Role-Permission mapping** — Admin has all permissions; User has read only

**Implementation in Express:**
```javascript
// Store role in JWT payload
const payload = { userId: 42, role: 'admin' };

// RBAC middleware
const authorize = (...roles) => (req, res, next) => {
  if (!roles.includes(req.user.role)) {
    return res.status(403).json({ error: 'Forbidden' });
  }
  next();
};

// Usage
app.delete('/users/:id', authenticate, authorize('admin'), deleteUser);
app.get('/reports', authenticate, authorize('admin', 'manager'), getReports);
```

---

### 🟡 Q48. What is CORS and why does it exist?
**Answer:**
**CORS (Cross-Origin Resource Sharing)** is a browser security mechanism that blocks JavaScript from making requests to a **different origin** (domain, protocol, or port) than the page it loaded from. This is the **Same-Origin Policy**.

**Why it exists:** Prevents malicious websites from making authenticated requests to your API on behalf of a logged-in user (CSRF-like attacks).

**How CORS works:**
1. Browser sends **preflight OPTIONS request** with `Origin` header
2. Server responds with `Access-Control-Allow-Origin`, `Access-Control-Allow-Methods`, etc.
3. If allowed, browser sends the actual request

**Express CORS:**
```javascript
const cors = require('cors');
app.use(cors({
  origin: ['https://myapp.com', 'https://admin.myapp.com'],
  methods: ['GET', 'POST', 'PUT', 'DELETE'],
  credentials: true
}));
```

---

### 🟡 Q49. What is Rate Limiting and how do you implement it?
**Answer:**
**Rate limiting** restricts the number of API requests a client (by IP, user, or API key) can make in a given time window. Protects against:
- Brute force attacks
- DDoS attacks
- API abuse
- Resource exhaustion

**Implementation with `express-rate-limit`:**
```javascript
const rateLimit = require('express-rate-limit');

const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100,                  // 100 requests per window
  message: { error: 'Too many requests, try again later' },
  standardHeaders: true,
  legacyHeaders: false,
});

app.use('/api/', limiter);

// Stricter limit for auth endpoints
const authLimiter = rateLimit({ windowMs: 60000, max: 5 });
app.use('/api/auth/', authLimiter);
```

**Advanced:** Use Redis for distributed rate limiting across multiple server instances.

---

### 🔴 Q50. What is the difference between Authentication and Authorization?
**Answer:**
- **Authentication (AuthN)** — Verifying **who you are**. "Are you who you claim to be?" Validates identity through credentials (password, biometrics, tokens).
- **Authorization (AuthZ)** — Verifying **what you can do**. "Are you allowed to do this?" Checks permissions after identity is confirmed.

**Example flow:**
1. User logs in with email/password → **Authentication**
2. Verified user tries to delete a post → Check if user is admin or post owner → **Authorization**

In Express: `authenticate` middleware verifies JWT → `authorize('admin')` middleware checks role.

---

## 8. Databases — PostgreSQL, MongoDB, MySQL, Redis

### 🟢 Q51. What is the difference between SQL and NoSQL databases?
**Answer:**
| Feature | SQL (PostgreSQL, MySQL) | NoSQL (MongoDB) |
|---------|------------------------|----------------|
| Structure | Tables with fixed schema | Documents, Key-Value, Graph |
| Schema | Rigid, predefined | Flexible, dynamic |
| Relationships | JOINs | Embedded documents or manual references |
| ACID | Full ACID compliance | Eventually consistent (some support ACID) |
| Scaling | Vertical (scale up) | Horizontal (scale out) |
| Best for | Complex queries, financial data | Flexible schema, real-time, big data |

**Use PostgreSQL/MySQL when:** Data is relational, transactions are critical (banking, inventory)
**Use MongoDB when:** Schema changes frequently, storing nested/hierarchical data, rapid prototyping

---

### 🟡 Q52. What are PostgreSQL indexes and when should you use them?
**Answer:**
An **index** is a data structure (usually B-tree) that allows faster data retrieval at the cost of extra storage and slower write operations.

**When to add indexes:**
- Columns frequently used in `WHERE` clauses
- Columns used in `JOIN` conditions
- Columns used in `ORDER BY` or `GROUP BY`
- Foreign keys

**Types of indexes:**
```sql
-- B-tree (default) — range queries, equality
CREATE INDEX idx_users_email ON users(email);

-- Unique index — enforces uniqueness
CREATE UNIQUE INDEX idx_users_username ON users(username);

-- Composite index — multiple columns
CREATE INDEX idx_orders_user_date ON orders(user_id, created_at);

-- Partial index — subset of rows
CREATE INDEX idx_active_users ON users(email) WHERE active = true;

-- Full-text search
CREATE INDEX idx_posts_content ON posts USING GIN(to_tsvector('english', content));
```

**Caution:** Over-indexing slows down INSERT/UPDATE/DELETE operations.

---

### 🟡 Q53. What is database normalization? Explain 1NF, 2NF, 3NF.
**Answer:**
**Normalization** is the process of organizing database tables to reduce redundancy and improve data integrity.

- **1NF (First Normal Form):** Each cell has a single atomic value; no repeating groups; primary key exists.
- **2NF (Second Normal Form):** Meets 1NF + all non-key attributes are **fully dependent** on the entire primary key (no partial dependencies).
- **3NF (Third Normal Form):** Meets 2NF + no transitive dependencies (non-key column depending on another non-key column).

**Example of 3NF violation:**
```
Orders: (order_id, customer_id, customer_city)
customer_city depends on customer_id, not order_id → violates 3NF
Fix: Move customer_city to Customers table
```

---

### 🟡 Q54. What is the difference between INNER JOIN, LEFT JOIN, RIGHT JOIN, and FULL OUTER JOIN?
**Answer:**
```sql
-- INNER JOIN — only matching rows from both tables
SELECT u.name, o.total FROM users u INNER JOIN orders o ON u.id = o.user_id;

-- LEFT JOIN — all rows from left table + matching from right (NULLs for no match)
SELECT u.name, o.total FROM users u LEFT JOIN orders o ON u.id = o.user_id;
-- Shows all users, even those with no orders

-- RIGHT JOIN — all rows from right table + matching from left
-- Full OUTER JOIN — all rows from both tables, NULLs where no match
```

**Venn Diagram analogy:**
- INNER = intersection
- LEFT = left circle + intersection
- RIGHT = right circle + intersection
- FULL OUTER = entire both circles

---

### 🟡 Q55. What is Redis and what are its common use cases?
**Answer:**
**Redis** (Remote Dictionary Server) is an in-memory data structure store supporting strings, hashes, lists, sets, sorted sets, and more. It's extremely fast (sub-millisecond) because all data lives in RAM.

**Common use cases:**
1. **Caching** — cache expensive DB queries or API responses
2. **Session storage** — store user sessions
3. **Rate limiting** — use INCR + EXPIRE for request counting
4. **Pub/Sub messaging** — real-time notifications
5. **Job queues** — BullMQ, Bull (Node.js queue libraries)
6. **Leaderboards** — sorted sets
7. **Distributed locking** — prevent race conditions

```javascript
// Caching example
const cached = await redis.get(`user:${id}`);
if (cached) return JSON.parse(cached);

const user = await db.query('SELECT * FROM users WHERE id = $1', [id]);
await redis.setex(`user:${id}`, 3600, JSON.stringify(user)); // 1 hour TTL
return user;
```

---

### 🔴 Q56. What is a database transaction and what are ACID properties?
**Answer:**
A **transaction** is a sequence of database operations executed as a single unit. Either ALL operations succeed or NONE do.

**ACID Properties:**
- **Atomicity** — All operations in a transaction succeed or all are rolled back ("all or nothing")
- **Consistency** — Database remains in a valid state before and after transaction (constraints not violated)
- **Isolation** — Concurrent transactions don't interfere with each other
- **Durability** — Once committed, data persists even on system failure (written to disk)

```sql
BEGIN;
  UPDATE accounts SET balance = balance - 500 WHERE id = 1;
  UPDATE accounts SET balance = balance + 500 WHERE id = 2;
COMMIT;  -- both succeed

-- If an error occurs:
ROLLBACK; -- neither change is applied
```

---

### 🔴 Q57. What are N+1 query problems and how do you fix them?
**Answer:**
The **N+1 problem** occurs when your code executes 1 query to fetch a list, then N additional queries for each item in the list.

```javascript
// N+1 Problem — 1 query for posts + N queries for each post's author
const posts = await db.query('SELECT * FROM posts'); // 1 query
for (const post of posts) {
  post.author = await db.query('SELECT * FROM users WHERE id = $1', [post.user_id]); // N queries
}
// Total: N+1 queries
```

**Solutions:**
1. **JOIN** — fetch related data in one query
```sql
SELECT posts.*, users.name FROM posts JOIN users ON posts.user_id = users.id;
```
2. **Batch loading (DataLoader)** — batch and cache individual lookups
3. **Eager loading** — ORM feature to load relations upfront (`include` in Prisma, `eager_load` in ActiveRecord)

---

## 9. Supabase

### 🟢 Q58. What is Supabase and how does it compare to Firebase?
**Answer:**
**Supabase** is an open-source **Backend-as-a-Service (BaaS)** built on PostgreSQL. It provides: database, authentication, real-time subscriptions, storage, and Edge Functions.

| Feature | Supabase | Firebase |
|---------|---------|---------|
| Database | PostgreSQL (relational) | Firestore (NoSQL) |
| Open-source | ✅ Yes | ❌ No |
| SQL support | ✅ Full SQL | ❌ No |
| Pricing | More affordable | Can get expensive |
| Vendor lock-in | Low (open-source) | High (Google) |
| Self-hosting | ✅ Yes | ❌ No |

---

### 🟡 Q59. How does Supabase Row Level Security (RLS) work?
**Answer:**
**RLS (Row Level Security)** is a PostgreSQL feature that restricts which rows a user can access based on policies. Supabase uses it to enforce data access rules at the database level.

```sql
-- Enable RLS on a table
ALTER TABLE posts ENABLE ROW LEVEL SECURITY;

-- Policy: users can only see their own posts
CREATE POLICY "Users see own posts"
  ON posts FOR SELECT
  USING (auth.uid() = user_id);

-- Policy: users can only update their own posts
CREATE POLICY "Users update own posts"
  ON posts FOR UPDATE
  USING (auth.uid() = user_id);
```

`auth.uid()` returns the currently authenticated Supabase user's ID. RLS is enforced server-side — even if someone accesses the API directly, they can't read others' rows.

---

## 10. Spring Boot & Java

### 🟢 Q60. What is Spring Boot and what problems does it solve?
**Answer:**
**Spring Boot** is an opinionated framework built on top of the Spring Framework that simplifies Java application development. It solves:
- **Auto-configuration** — automatically configures Spring beans based on classpath
- **Embedded server** — ships with embedded Tomcat/Jetty; no separate WAR deployment needed
- **Dependency management** — Spring Boot starters bundle compatible dependencies
- **Production-ready features** — metrics, health checks, externalized configuration (Actuator)
- **Eliminates boilerplate** — no XML configuration required

```java
@SpringBootApplication
public class MyApp {
    public static void main(String[] args) {
        SpringApplication.run(MyApp.class, args);
    }
}
```

---

### 🟡 Q61. What are the key Spring Boot annotations and what do they do?
**Answer:**
| Annotation | Purpose |
|-----------|---------|
| `@SpringBootApplication` | Combines @Configuration + @ComponentScan + @EnableAutoConfiguration |
| `@RestController` | Marks class as REST controller; combines @Controller + @ResponseBody |
| `@RequestMapping` | Maps HTTP requests to handler methods |
| `@GetMapping` / `@PostMapping` | Shorthand for GET/POST request mapping |
| `@Service` | Marks a service class (business logic layer) |
| `@Repository` | Marks a DAO class (data access layer) |
| `@Component` | Generic Spring-managed component |
| `@Autowired` | Dependency injection — Spring injects the bean |
| `@Entity` | Marks class as JPA entity (maps to DB table) |
| `@Value("${prop}")` | Injects value from application.properties |
| `@Transactional` | Wraps method in a DB transaction |

---

### 🟡 Q62. What is Dependency Injection (DI) and Inversion of Control (IoC) in Spring?
**Answer:**
**IoC (Inversion of Control):** Instead of objects creating their own dependencies, the control of object creation is inverted to a container (Spring's IoC container).

**Dependency Injection (DI):** The mechanism by which IoC is achieved. Dependencies are "injected" into a class rather than the class creating them.

```java
// Without DI — tightly coupled
public class OrderService {
    private UserRepository repo = new UserRepository(); // creates dependency
}

// With DI — loosely coupled
@Service
public class OrderService {
    private final UserRepository repo; // dependency declared

    @Autowired // Spring injects the implementation
    public OrderService(UserRepository repo) {
        this.repo = repo;
    }
}
```

**Benefits:** Testability (easy mocking), loose coupling, single responsibility.

---

### 🔴 Q63. How did you deploy your Spring Boot app on AWS EC2 with RDS and PostgreSQL?
**Answer (Project-based):**
**Architecture:**
1. **EC2 (Ubuntu)** — hosts the Spring Boot JAR
2. **RDS (PostgreSQL)** — managed database in private subnet
3. **Security Groups** — EC2 security group allows port 8080/443; RDS security group allows port 5432 only from EC2's security group
4. **Nginx** — reverse proxy on EC2 forwarding port 80/443 to app's port 8080
5. **SSL** — Let's Encrypt certificate via Certbot on Nginx

**`application.properties` on EC2:**
```properties
spring.datasource.url=jdbc:postgresql://<rds-endpoint>:5432/mydb
spring.datasource.username=${DB_USER}
spring.datasource.password=${DB_PASS}
```

**Run command:**
```bash
nohup java -jar myapp.jar --spring.profiles.active=prod &
```

---

## 11. Cloud & AWS

### 🟢 Q64. What is the difference between EC2, RDS, and S3 in AWS?
**Answer:**
| Service | Type | Purpose |
|---------|------|---------|
| **EC2** | Compute | Virtual server — run applications, code, web servers |
| **RDS** | Database | Managed relational database (PostgreSQL, MySQL, Aurora) |
| **S3** | Storage | Object storage — files, images, backups, static websites |
| **VPC** | Networking | Virtual private network to isolate resources |
| **IAM** | Security | Identity and Access Management — roles and permissions |
| **Route 53** | DNS | Domain name management and routing |
| **CloudFront** | CDN | Content delivery network — cache static assets globally |

---

### 🟡 Q65. What is a VPC and why is it important for security?
**Answer:**
**VPC (Virtual Private Cloud)** is an isolated virtual network in AWS where you launch resources. Key components:
- **Subnets** — Public (internet-accessible) and Private (no direct internet)
- **Internet Gateway** — Allows public subnet resources to access internet
- **NAT Gateway** — Allows private subnet resources to initiate outbound internet (not inbound)
- **Security Groups** — Stateful firewall at instance level (what traffic is allowed)
- **NACLs** — Stateless firewall at subnet level

**Best practice architecture:**
- Web/App servers in **public subnet** (EC2 with Nginx)
- Databases in **private subnet** (RDS) — only accessible from app servers
- Bastion host for SSH access to private subnet

---

### 🟡 Q66. What is Nginx and how do you configure it as a reverse proxy?
**Answer:**
**Nginx** is a high-performance web server commonly used as a **reverse proxy** (forwarding requests from port 80/443 to your app), **load balancer**, and **static file server**.

```nginx
server {
    listen 80;
    server_name myapp.com;
    return 301 https://$server_name$request_uri; # Redirect HTTP → HTTPS
}

server {
    listen 443 ssl;
    server_name myapp.com;

    ssl_certificate /etc/letsencrypt/live/myapp.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/myapp.com/privkey.pem;

    location / {
        proxy_pass http://localhost:3000;     # Forward to Node.js app
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
}
```

---

### 🔴 Q67. What is PM2 and why use it for Node.js in production?
**Answer:**
**PM2 (Process Manager 2)** is a production process manager for Node.js that provides:

- **Keep-alive** — automatically restarts crashed apps
- **Cluster mode** — runs multiple instances across CPU cores for load balancing
- **Log management** — centralized logs per app
- **Zero-downtime reload** — `pm2 reload app` — no dropped connections
- **Monitoring** — CPU and memory usage dashboard
- **Startup scripts** — `pm2 startup` generates init script so apps restart on server reboot

```bash
pm2 start app.js --name "my-api"
pm2 start app.js -i max        # Cluster mode — use all CPU cores
pm2 list                        # View running apps
pm2 logs my-api                 # Tail logs
pm2 reload my-api               # Zero-downtime reload
pm2 startup                     # Generate startup script
pm2 save                        # Save current process list
```

---

## 12. DevOps — Docker, CI/CD, GitHub Actions, Nginx, PM2

### 🟢 Q68. What is Docker and why is it used?
**Answer:**
**Docker** is a containerization platform that packages applications and all their dependencies (code, runtime, libraries, config) into a **container** — a lightweight, portable, isolated environment. Containers run identically regardless of the host OS.

**Problems Docker solves:**
- "It works on my machine" — containers ensure consistent environments
- Dependency conflicts between projects
- Complex deployment setup
- Easier scaling and orchestration

**Key concepts:**
- **Image** — read-only blueprint (like a class)
- **Container** — running instance of an image (like an object)
- **Dockerfile** — instructions to build an image
- **Registry** — stores images (Docker Hub, AWS ECR)

---

### 🟡 Q69. Write a production Dockerfile for a Node.js application.
**Answer:**
```dockerfile
# Multi-stage build — keeps final image small
FROM node:20-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production

FROM node:20-alpine AS production
WORKDIR /app

# Security: run as non-root user
RUN addgroup -S appgroup && adduser -S appuser -G appgroup

COPY --from=builder /app/node_modules ./node_modules
COPY . .

RUN chown -R appuser:appgroup /app
USER appuser

EXPOSE 3000
ENV NODE_ENV=production

CMD ["node", "src/index.js"]
```

**Best practices:**
- Use `alpine` for small image size
- Use `.dockerignore` to exclude `node_modules`, `.git`
- Multi-stage builds to separate build and runtime
- Non-root user for security
- Use `npm ci` (clean install) instead of `npm install` for reproducibility

---

### 🟡 Q70. What is Docker Compose and when do you use it?
**Answer:**
**Docker Compose** is a tool for defining and running **multi-container Docker applications** using a `docker-compose.yml` file. Use it for local development and testing of multi-service applications.

```yaml
version: '3.8'
services:
  api:
    build: .
    ports:
      - "3000:3000"
    environment:
      - DATABASE_URL=postgresql://user:pass@db:5432/mydb
      - REDIS_URL=redis://redis:6379
    depends_on:
      - db
      - redis

  db:
    image: postgres:15
    environment:
      POSTGRES_USER: user
      POSTGRES_PASSWORD: pass
      POSTGRES_DB: mydb
    volumes:
      - postgres_data:/var/lib/postgresql/data

  redis:
    image: redis:7-alpine

volumes:
  postgres_data:
```
```bash
docker-compose up -d    # Start in background
docker-compose down     # Stop and remove containers
docker-compose logs -f  # Follow logs
```

---

### 🟡 Q71. Explain a CI/CD pipeline using GitHub Actions.
**Answer:**
**CI/CD (Continuous Integration/Continuous Deployment)** automates testing and deployment when code is pushed.

```yaml
# .github/workflows/deploy.yml
name: CI/CD Pipeline

on:
  push:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '20'
      - run: npm ci
      - run: npm test
      - run: npm run lint

  deploy:
    needs: test  # Only deploy if tests pass
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Build and push Docker image
        run: |
          docker build -t myapp:${{ github.sha }} .
          docker push myregistry/myapp:${{ github.sha }}

      - name: Deploy to EC2
        uses: appleboy/ssh-action@v1
        with:
          host: ${{ secrets.EC2_HOST }}
          username: ubuntu
          key: ${{ secrets.EC2_SSH_KEY }}
          script: |
            docker pull myregistry/myapp:${{ github.sha }}
            docker stop myapp || true
            docker run -d --name myapp -p 3000:3000 myregistry/myapp:${{ github.sha }}
```

---

### 🔴 Q72. What is the difference between Continuous Integration, Continuous Delivery, and Continuous Deployment?
**Answer:**
- **Continuous Integration (CI)** — Developers frequently merge code to main branch; automated tests run on every push to catch integration issues early.
- **Continuous Delivery (CD)** — Code is always in a deployable state; deployment to production is manual (one click), but all automation up to that point is done.
- **Continuous Deployment** — Every code change that passes tests is automatically deployed to production with **no human intervention**.

**Pipeline stages:** Code Push → Build → Test (Unit, Integration, E2E) → Security Scan → Staging Deploy → Smoke Tests → Production Deploy

---

## 13. Automation — n8n, Webhooks, Cron Jobs

### 🟡 Q73. What is n8n and what types of workflows did you build with it?
**Answer:**
**n8n** is an open-source, self-hostable workflow automation tool similar to Zapier or Make. It uses a visual node-based interface where each node represents an action (HTTP request, database query, message send, transformation, etc.).

**Workflows built in my experience:**
- **AI Video Pipeline:** Trigger (Telegram message) → OpenAI text generation → FFmpeg video processing → Upload to cloud storage → Send delivery notification via Telegram bot
- **Automation benefits:** No custom polling code, visual debugging, built-in retry logic, error handling, scheduling

**Key n8n concepts:**
- **Trigger nodes** — Start workflows (Webhook, Cron, Manual)
- **Action nodes** — Perform operations (HTTP Request, OpenAI, Postgres, Email)
- **Logic nodes** — Control flow (IF, Switch, Merge, Loop)
- **Credentials** — Secure API key storage

---

### 🟡 Q74. What is a Webhook and how does it differ from polling?
**Answer:**
**Webhook (Push model):** The server sends data to your endpoint automatically when an event occurs. You register a URL; the source calls it when something happens.

**Polling (Pull model):** Your app periodically queries the source API to check for new data.

| | Webhook | Polling |
|-|---------|---------|
| Direction | Source pushes data to you | You request data from source |
| Real-time | Near real-time | Delayed (interval-dependent) |
| Efficiency | Very efficient | Wasteful (many empty requests) |
| Complexity | Need public endpoint | Simpler to implement |

```javascript
// Webhook receiver in Express
app.post('/webhook/telegram', (req, res) => {
  const { message } = req.body;
  handleTelegramMessage(message);
  res.status(200).send('OK');
});
```

---

### 🟡 Q75. How do you implement Cron Jobs in Node.js?
**Answer:**
**Cron jobs** are scheduled tasks using the cron syntax: `* * * * *` (minute, hour, day-of-month, month, day-of-week).

```javascript
const cron = require('node-cron');

// Run every day at midnight
cron.schedule('0 0 * * *', async () => {
  await cleanupExpiredSessions();
  console.log('Daily cleanup done');
});

// Run every 30 minutes
cron.schedule('*/30 * * * *', async () => {
  await syncExternalData();
});

// Run every Monday at 9 AM
cron.schedule('0 9 * * 1', async () => {
  await sendWeeklyReport();
});
```

**Cron expression reference:**
```
┌───────────── minute (0-59)
│ ┌───────────── hour (0-23)
│ │ ┌───────────── day of month (1-31)
│ │ │ ┌───────────── month (1-12)
│ │ │ │ ┌───────────── day of week (0-7, 0/7=Sunday)
* * * * *
```

---

### 🔴 Q76. How did you integrate OpenAI APIs in your automation workflows?
**Answer:**
**Integration patterns:**
```javascript
// Chat completion with OpenAI SDK
const OpenAI = require('openai');
const client = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

async function generateVideoScript(topic) {
  const response = await client.chat.completions.create({
    model: 'gpt-4o',
    messages: [
      { role: 'system', content: 'You are a video script writer.' },
      { role: 'user', content: `Write a 60-second script about: ${topic}` }
    ],
    max_tokens: 500,
    temperature: 0.7
  });
  return response.choices[0].message.content;
}
```

**Best practices:**
- Store API key in environment variables, never in code
- Implement retry logic with exponential backoff for rate limits
- Use streaming for real-time responses (`stream: true`)
- Cache results where appropriate to reduce costs
- Track token usage for cost management

---

## 14. System Design

### 🟡 Q77. How would you design a URL shortener system?
**Answer:**
**Requirements:** Convert long URLs to short codes; redirect short URL to original.

**Components:**
1. **API Server** — POST `/shorten` (create), GET `/:code` (redirect)
2. **Database** — Store `{ code, original_url, created_at, click_count }`
3. **Cache (Redis)** — Cache code → URL mapping for fast lookups

**Short code generation:**
- Generate random 6-char alphanumeric string
- Check DB for collision; regenerate if exists
- Alternative: Base62 encode auto-increment ID

**Schema:**
```sql
CREATE TABLE urls (
  id SERIAL PRIMARY KEY,
  code VARCHAR(10) UNIQUE NOT NULL,
  original_url TEXT NOT NULL,
  created_at TIMESTAMP DEFAULT NOW(),
  expires_at TIMESTAMP,
  click_count INTEGER DEFAULT 0
);
CREATE INDEX idx_code ON urls(code);
```

**Redirect flow:**
1. GET `/:code` → check Redis cache first
2. Cache miss → query DB, store in Redis with TTL
3. Return 301 (permanent) or 302 (temporary) redirect

---

### 🔴 Q78. How would you design a scalable REST API that handles millions of requests?
**Answer:**
**Layers of scalability:**

1. **Load Balancing** (Nginx/AWS ALB) — distribute traffic across multiple server instances
2. **Horizontal scaling** — run multiple app instances (PM2 cluster mode, Docker + Kubernetes)
3. **Caching** (Redis) — cache expensive queries, reduce DB load
4. **Database optimization** — indexes, connection pooling (PgBouncer), read replicas for reads
5. **CDN** — serve static assets from edge locations
6. **Async processing** — offload heavy tasks to job queues (Bull + Redis)
7. **Rate limiting** — prevent abuse, protect capacity
8. **API Gateway** — centralized auth, rate limiting, request routing
9. **Database sharding/partitioning** — for extreme scale
10. **Monitoring** — APM tools (Datadog, New Relic), centralized logging (ELK stack)

---

### 🔴 Q79. What is the difference between vertical and horizontal scaling?
**Answer:**
- **Vertical Scaling (Scale Up):** Adding more resources (CPU, RAM, disk) to a single server. Simpler but has a physical limit and creates a single point of failure. Expensive at high end.
- **Horizontal Scaling (Scale Out):** Adding more servers and distributing the load. More complex but virtually unlimited scale, fault tolerant, cost-effective with commodity hardware.

**For stateless apps** (well-designed REST APIs): horizontal scaling is ideal — just add more instances behind a load balancer.
**Stateful considerations:** Session data must be shared (use Redis for sessions), not stored in-memory on one server.

---

### 🔥 Q80. Explain the CAP Theorem and how it applies to your database choices.
**Answer:**
**CAP Theorem** states that a distributed system can only guarantee **2 of 3** properties simultaneously:

- **Consistency (C)** — Every read receives the most recent write or an error
- **Availability (A)** — Every request receives a response (not necessarily latest data)
- **Partition Tolerance (P)** — System continues operating despite network partitions (dropped messages)

Since network partitions are inevitable in distributed systems, **P is always required**, leaving you to choose between C and A:

- **CP systems** (prioritize Consistency): PostgreSQL, MongoDB (strong mode), Redis — return error rather than stale data during partition
- **AP systems** (prioritize Availability): Cassandra, DynamoDB, CouchDB — return potentially stale data, guarantee availability

**Practical impact:**
- PostgreSQL (your choice) is CP — transactions guarantee consistency at potential availability cost
- Redis as cache is AP — may serve slightly stale cached data but always responds

---

## 15. Project-Based Questions

### 🟡 Q81. Walk me through your AI Video Automation System.
**Answer:**
**System overview:** An AI-powered automation pipeline triggered via Telegram bot.

**End-to-end flow:**
1. **User sends video topic** via Telegram message to the bot
2. **n8n webhook** receives the Telegram message
3. **OpenAI API** generates video script/narration text
4. **TTS (Text-to-Speech)** converts text to audio (e.g., ElevenLabs or OpenAI TTS)
5. **FFmpeg** processes video: overlays subtitles, merges audio with video, applies filters
6. **Cloud storage** receives the processed video file
7. **Telegram bot** delivers the final video back to the user

**Key technical decisions:**
- n8n for orchestration (visual workflow debugging, built-in retry)
- FFmpeg for efficient server-side media processing
- Async processing to handle long video generation without timeout
- Webhook-based (not polling) for real-time Telegram updates

---

### 🟡 Q82. Explain your PDF Generation System using Puppeteer.
**Answer:**
**Problem solved:** Generating complex, multi-page engineering reports as PDFs with precise formatting that browsers can't achieve with simple print CSS.

**Architecture:**
1. **Frontend (Next.js)** — User fills report data in a form
2. **API endpoint** receives report data as JSON
3. **Puppeteer** launches headless Chrome, renders a dedicated HTML template with the data
4. **`page.pdf()`** generates the PDF from the rendered page
5. **PDF merging** (pdf-lib or PyPDF2) combines multiple rendered pages if needed
6. **Response** sends PDF file to client for download

```javascript
const puppeteer = require('puppeteer');

async function generatePDF(reportData) {
  const browser = await puppeteer.launch({ args: ['--no-sandbox'] });
  const page = await browser.newPage();
  
  const html = renderReportTemplate(reportData);
  await page.setContent(html, { waitUntil: 'networkidle0' });
  
  const pdf = await page.pdf({
    format: 'A4',
    printBackground: true,
    margin: { top: '20mm', bottom: '20mm', left: '15mm', right: '15mm' }
  });
  
  await browser.close();
  return pdf;
}
```

**Challenge solved:** Multi-page rendering with page breaks and consistent headers/footers.

---

### 🔴 Q83. How did you implement and deploy the Spring Boot app on AWS?
**Answer:**
**Full deployment architecture:**

```
User → Route 53 (DNS) → EC2 (Nginx, port 443)
                              ↓ reverse proxy
                         Spring Boot JAR (port 8080)
                              ↓ JDBC
                         RDS PostgreSQL (private subnet, port 5432)
```

**Steps taken:**
1. Launched **EC2 (Ubuntu 22.04)** in public subnet; installed Java 17
2. Created **RDS PostgreSQL** in private subnet; configured security group to allow port 5432 from EC2 security group only
3. Built Spring Boot JAR: `./mvnw package -DskipTests`
4. Uploaded JAR to EC2 via SCP/CI pipeline
5. Configured `application.properties` with RDS connection string (credentials from environment variables)
6. Set up **Nginx** as reverse proxy with SSL (Certbot + Let's Encrypt)
7. Created systemd service for auto-restart on crash/reboot
8. Configured **VPC** networking, security groups, and NACLs

---

## 16. Behavioral & HR Questions

### Q84. Tell me about yourself.
**Answer (tailored):**
"I'm a Full Stack Engineer with hands-on experience building scalable backend systems, automation workflows, and cloud-deployed applications. At Gixi AI, I built AI-powered automation using n8n, OpenAI, and Telegram bots, while managing Linux VPS deployments with Docker, Nginx, and GitHub Actions CI/CD. I've worked across the full stack — Next.js and React on the frontend, Node.js and Spring Boot on the backend, and PostgreSQL/MongoDB for data. I'm particularly passionate about backend architecture, automation pipelines, and production deployments. I enjoy owning systems end-to-end — from writing the code to deploying and monitoring it in production."

---

### Q85. Describe a challenging technical problem you solved.
**Answer structure (STAR: Situation, Task, Action, Result):**
**Situation:** While building the AI Video Automation System, the video generation took 2-5 minutes but our webhook response had to return within 30 seconds or Telegram would mark it as failed.

**Task:** Prevent timeout while maintaining status updates to the user.

**Action:** Implemented an async job pattern — webhook immediately returned 200 OK to Telegram, added the video job to a Bull/Redis queue, background worker processed the video, and upon completion sent the result directly back to the user via Telegram Bot API.

**Result:** Webhook never timed out, users received their videos after processing, system handled concurrent video requests without blocking.

---

### Q86. How do you handle a production outage?
**Answer:**
1. **Stay calm** — don't make changes under panic that create new problems
2. **Assess impact** — understand what's down, how many users affected, severity
3. **Check logs first** — `pm2 logs`, `journalctl -u nginx`, `docker logs`
4. **Check recent changes** — was there a recent deployment? Roll back if needed
5. **Check infrastructure** — server health (CPU, RAM, disk), database connections, external dependencies
6. **Communicate** — notify stakeholders of the issue and ETA
7. **Fix or rollback** — fastest path to restore service
8. **Post-mortem** — after resolution, document root cause, impact, and prevention measures (monitoring alerts, better error handling)

---

### Q87. What is your approach to writing clean, maintainable code?
**Answer:**
1. **Meaningful naming** — variables, functions, classes should explain their purpose
2. **Single Responsibility Principle** — each function/module does one thing well
3. **DRY (Don't Repeat Yourself)** — extract reusable abstractions
4. **Small functions** — easier to test and understand
5. **Error handling** — always handle edge cases and errors explicitly
6. **Comments for WHY not WHAT** — code should be self-explanatory; comments explain non-obvious reasoning
7. **Consistent code style** — ESLint, Prettier for formatting
8. **Write tests** — unit tests document expected behavior and prevent regressions
9. **Code reviews** — fresh eyes catch blind spots
10. **Refactor incrementally** — leave code cleaner than you found it (Boy Scout Rule)

---

### Q88. Where do you see yourself in 3–5 years?
**Answer:**
"In the next 3-5 years, I want to deepen my expertise in distributed systems and cloud architecture — moving from building single applications to designing systems that scale to millions of users. I'm particularly interested in backend specialization: event-driven architectures, microservices, and infrastructure as code. I'd also love to take on a tech lead or senior engineer role where I can mentor others while continuing to drive technical decisions. Long-term, I'm drawn to solving complex performance and scalability problems in a product-focused engineering team."

---

### Q89. How do you stay updated with rapidly changing technology?
**Answer:**
- **Follow official docs and changelogs** — Next.js blog, Node.js releases, AWS announcements
- **Technical newsletters** — JavaScript Weekly, Node Weekly, Bytes
- **Build side projects** — hands-on learning with new tools (that's how I learned n8n, Supabase, Docker)
- **GitHub trending** — discover emerging tools and libraries
- **YouTube/Courses** — Fireship, Traversy Media, freeCodeCamp for visual learning
- **Community** — dev.to, Hashnode, Stack Overflow, Reddit (r/node, r/webdev)
- **Code at work** — applying new patterns and tools in real projects cements learning

---

### Q90. Why should we hire you?
**Answer:**
"Because I don't just write code — I own systems. I've built and deployed full-stack applications end-to-end: from designing the database schema to writing the API, deploying it on AWS with Docker and Nginx, setting up CI/CD with GitHub Actions, and monitoring it in production with PM2. I bring practical experience with automation (n8n, OpenAI, Webhooks), security (JWT, RBAC, CORS, rate limiting), and cloud infrastructure — skills that let me work independently and deliver production-ready software. I'm a fast learner who picks up new tools quickly, and I'm genuinely excited about solving hard problems. You'd be getting someone who takes full ownership and cares deeply about code quality and system reliability."

---

> 💡 **Interview Tips:**
> - Always relate answers to your **actual project experience**
> - Use the **STAR format** (Situation, Task, Action, Result) for behavioral questions
> - It's okay to say "I haven't used that specific tool, but here's how I'd approach it using what I know"
> - Ask thoughtful questions at the end: team structure, tech stack, deployment process, on-call expectations
> - Quantify your impact where possible: "reduced load time by 40%", "handled 10k concurrent users"

---

*Prepared specifically for Ubaid Bin Waris | Full Stack Engineer Interview Preparation*
