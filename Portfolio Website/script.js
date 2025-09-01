function toggleMenu() {
  const menu = document.querySelector(".menu-links");
  const icon = document.querySelector(".hamburger-icon");
  menu.classList.toggle("open");
  icon.classList.toggle("open");
}

// For filtering projects in projects.html.
const buttons = document.querySelectorAll(".project-tabs button");
const posts = document.querySelectorAll(".project-post");

if (buttons.length && posts.length) {
  buttons.forEach(button => {
    button.addEventListener("click", () => {
      buttons.forEach(btn => btn.classList.remove("active"));
      button.classList.add("active");

      const category = button.dataset.category;

      posts.forEach(post => {
        if (category === "all" || post.classList.contains(category)) {
          post.style.display = "block";
        } else {
          post.style.display = "none";
        }
      });
    });
  });

  // Back to top arrow logic
  const backToTop = document.querySelector('.back-to-top');

  if (backToTop) {
    const onScroll = () => {
      backToTop.classList.toggle('show', window.scrollY > 300);
    };
    window.addEventListener('scroll', onScroll, { passive: true });
    onScroll(); // set initial state on load

    backToTop.addEventListener('click', (e) => {
      e.preventDefault();
      window.scrollTo({ top: 0, behavior: 'smooth' });
    });
  }
}